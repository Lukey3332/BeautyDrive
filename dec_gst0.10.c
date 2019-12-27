#include <stdint.h>
#include "dec.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <string.h>
#include <math.h>
#include "sys.h"
#include "vid.h"

static void pad_added_handler (GstElement *src, GstPad *new_pad, void * tmp);
static GstElement* app_sink;
static GstElement* pipeline;
static GstBus* bus;
static uint currentFrame = 1;
static surface * background_surf;
static int blit_scaled = 0;

void Dec_Init ()
{
	gst_init (&Sys_Args.argc, &Sys_Args.argv);
}

void Dec_LoadBackground (const char * url)
{
	pipeline = gst_pipeline_new("Background-Pipe");
	bus = gst_element_get_bus (GST_ELEMENT (pipeline));
	GstElement* source;
	if (url == NULL) {
		// Using the "video test source"
		Sys_Printf("Using debug Video Source");
		source = gst_element_factory_make ("videotestsrc", "source");
		g_object_set (source, "pattern", 18, NULL);
	} else {
		// Using an actual video file
		source = gst_element_factory_make ("uridecodebin", "source");
		// Warning: the URI must be absolute.
		g_object_set(source, "uri", url, NULL);
		g_signal_connect (source, "pad-added", G_CALLBACK (pad_added_handler), NULL);
	}
	
	if (source == NULL) {
		Sys_Error("Unable to create source element.\n");
	}
	if (!gst_bin_add(GST_BIN(pipeline), source)) {
		Sys_Error("Unable to add source element to the pipeline.\n");
	}
	
	app_sink = gst_element_factory_make ("appsink", "app_sink");
	
	gst_app_sink_set_max_buffers( (GstAppSink*)app_sink, 2);
	gst_base_sink_set_sync( (GstBaseSink*)app_sink, FALSE);
	
	
	if (app_sink == NULL) {
		Sys_Error("Unable to create sink element.\n");
	}
	if (!gst_bin_add(GST_BIN(pipeline), app_sink)) {
		Sys_Error("Unable to add sink element to the pipeline.\n");
	}
	
	if (url == NULL) {
		// Using the "videotestsrc" - static connection
		if (!gst_element_link(source, (GstElement*)app_sink)) {
			Sys_Error("Unable to link the source to the sink.\n");
		}
	}
	
	if (gst_element_set_state(pipeline, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
		Sys_Error("Unable to set the pipeline to the playing state.\n");
	}
	
	gst_element_get_state (pipeline, NULL, NULL, -1);
	
	//gst_element_send_event (pipeline, gst_event_new_step (GST_FORMAT_BUFFERS, 2, 1.0, TRUE, FALSE));
	//gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_STEP_DONE);
	//gst_element_get_state (pipeline, NULL, NULL, -1);
	
	GstBuffer* sample = gst_app_sink_pull_preroll((GstAppSink*)app_sink);
	
	// Get the frame format.
	GstCaps * caps = gst_buffer_get_caps (sample);
	if (!caps) {
		Sys_Error("could not get snapshot format\n");
	}
	
	gint width, height;
	GstStructure * s = gst_caps_get_structure (caps, 0);
	int res = gst_structure_get_int (s, "width", &width)
		| gst_structure_get_int (s, "height", &height);
	if (!res) {
		Sys_Error("could not get snapshot dimension\n");
	}
	background_surf = Vid_CreateSurface( YUV, 0, width, height);
	
	if( abs(width - BASEWIDTH) > 10 || abs(height - BASEHEIGHT) > 10 )
		blit_scaled = 1;
	else
		blit_scaled = 0;
	
	void * dest_buffer = Vid_GetAndLockBuffer( YUV, background_surf );
	memcpy( dest_buffer, sample->data, sample->size );
	Vid_UnlockBuffer( YUV, background_surf );
	
	gst_buffer_unref( sample );
}

uint Dec_Update (uint targetFrame)
{
	GstBuffer* sample;
	
	//gint64 current;
	//gst_element_query_position (pipeline, GST_FORMAT_TIME, &current);
	//Sys_Printf("Bytes: %u", current);
	
	if( targetFrame == currentFrame ) return currentFrame;
	
	if( ((int) targetFrame - (int) currentFrame) < 0 ) return currentFrame;
	
	gst_element_send_event (pipeline, gst_event_new_step (GST_FORMAT_BUFFERS, abs(targetFrame - currentFrame), 1.0, TRUE, FALSE));
	gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_STEP_DONE);
	gst_element_get_state (pipeline, NULL, NULL, -1);
	currentFrame = targetFrame;
	
	sample = gst_app_sink_pull_preroll( (GstAppSink*)app_sink );
	
	if(!sample) return currentFrame;
	
	void * dest_buffer = Vid_GetAndLockBuffer( YUV, background_surf );
	memcpy( dest_buffer, sample->data, sample->size );
	Vid_UnlockBuffer( YUV, background_surf );
	
	gst_buffer_unref( sample );
	return currentFrame;
}

void Dec_DrawBackground ()
{
	if(blit_scaled) Vid_BlitBufferScaled( YUV, background_surf, 0, 0, BASEWIDTH, BASEHEIGHT);
	else Vid_BlitBuffer( YUV, background_surf, 0, 0);
}

int Dec_Seek (seekT type, int pos)
{
	gint64 current;
	GstFormat format;
	switch(type) {
		case REWIND:
			if (!gst_element_seek(app_sink, 1.0, GST_FORMAT_BUFFERS, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
			GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
				Sys_Error("Seek failed!\n");
				return 1;
			}
		break;
		
		case SEEK:
			if (!gst_element_seek(app_sink, 1.0, GST_FORMAT_BUFFERS, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
			GST_SEEK_TYPE_SET, pos, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
				Sys_Error("Seek failed!\n");
				return 1;
			}
		break;
		
		case DIRECTION:
			format = GST_FORMAT_TIME;
			gst_element_query_position (app_sink, &format, &current);
			if (!gst_element_seek(app_sink, pos, format, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
			GST_SEEK_TYPE_SET, current, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
				Sys_Error("Seek failed!\n");
				return 1;
			}
		break;
	}
	return 0;
}

void Dec_Shutdown ()
{
	
}

static void pad_added_handler (GstElement *src, GstPad *new_pad, void * tmp)
{
	GstPadLinkReturn ret;
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;
	/* Check the new pad's type */
	new_pad_caps = gst_pad_get_caps_reffed (new_pad);
	new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
	new_pad_type = gst_structure_get_name (new_pad_struct);
	
	// We are only interested in the video pad. Look for it...
	if (g_str_has_prefix (new_pad_type, "video/x-raw"))
	{
		/* Attempt the link */
		GstPad *sink_pad_video = gst_element_get_static_pad (app_sink, "sink");
		ret = gst_pad_link (new_pad, sink_pad_video);
		if (GST_PAD_LINK_FAILED (ret)) {
			Sys_Error("  Type is '%s' but link failed.\n", new_pad_type);
		}
		
		/* Unreference the sink pad */
		gst_object_unref (sink_pad_video);
	}
	
	gst_caps_unref (new_pad_caps);
}

