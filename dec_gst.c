#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <string.h>
#include "sys.h"
#include "vid.h"

static GstElement* app_sink;
static void pad_added_handler (GstElement *src, GstPad *new_pad, void * tmp);
static Vid_SurfaceRef background_surf;

void Dec_Init ()
{
	gst_init (&sys_params.argc, &sys_params.argv);
}

void Dec_LoadBackground (const char * url)
{
	GstElement* pipeline = gst_pipeline_new("Background-Pipe");
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
	
	gst_app_sink_set_max_buffers( (GstAppSink*)app_sink, 5);
	gst_base_sink_set_sync( (GstBaseSink *)app_sink, FALSE);
	
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
	
	if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
		Sys_Error("Unable to set the pipeline to the playing state.\n");
	}
	
	GstSample* sample = gst_app_sink_pull_sample((GstAppSink*)app_sink);
	
	// Get the frame format.
	GstCaps * caps = gst_sample_get_caps (sample);
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
	background_surf = Vid_CreateSurface(width, height, YUV);
	
	gst_sample_unref(sample);
}

void Dec_DrawBackground ()
{
	GstSample* sample;
retry:
	sample = gst_app_sink_pull_sample((GstAppSink*)app_sink);
	if(!sample) {
		// Finished playing.
		if (!gst_element_seek(app_sink, 1.0, GST_FORMAT_DEFAULT, GST_SEEK_FLAG_FLUSH,
			GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
			Sys_Error("Seek failed!\n");
		}
		goto retry;
	}
	
	GstBuffer * vid_buffer = gst_sample_get_buffer(sample);
	GstMapInfo data;
	gst_buffer_map (vid_buffer, &data, GST_MAP_READ);
	void * dest_buffer = Vid_GetAndLockBuffer(background_surf);
	memcpy( dest_buffer, data.data, data.size);
	Vid_UnlockBuffer(background_surf);
	gst_buffer_unmap (vid_buffer, &data); 
	
	gst_sample_unref(sample);
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

	g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

	/* Check the new pad's type */
	new_pad_caps = gst_pad_get_current_caps (new_pad);
	new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
	new_pad_type = gst_structure_get_name (new_pad_struct);
	
	// We are only interested in the video pad. Look for it...
	if (g_str_has_prefix (new_pad_type, "video/x-raw"))
	{
		/* Attempt the link */
		GstPad *sink_pad_video = gst_element_get_static_pad (app_sink, "sink");
		ret = gst_pad_link (new_pad, sink_pad_video);
		if (GST_PAD_LINK_FAILED (ret)) {
			g_print ("  Type is '%s' but link failed.\n", new_pad_type);
		} else {
			g_print ("  Link succeeded (type '%s').\n", new_pad_type);
		} 
		
		/* Unreference the sink pad */
		gst_object_unref (sink_pad_video);
	} else {
		g_print ("  It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
		gst_caps_unref (new_pad_caps);
	}
}

