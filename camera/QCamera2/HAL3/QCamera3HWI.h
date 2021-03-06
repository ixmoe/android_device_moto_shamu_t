/* Copyright (c) 2012-2015, The Linux Foundataion. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#ifndef __QCAMERA3HARDWAREINTERFACE_H__
#define __QCAMERA3HARDWAREINTERFACE_H__

#include <pthread.h>
#include <utils/List.h>
#include <utils/KeyedVector.h>
#include <hardware/camera3.h>
#include <camera/CameraMetadata.h>
#include "QCamera3HALHeader.h"
#include "QCamera3Channel.h"

#include <hardware/power.h>

extern "C" {
#include <mm_camera_interface.h>
#include <mm_jpeg_interface.h>
}
#ifdef CDBG
#undef CDBG
#endif //#ifdef CDBG
#define CDBG(fmt, args...) ALOGD_IF(gCamHal3LogLevel >= 2, fmt, ##args)

#ifdef CDBG_HIGH
#undef CDBG_HIGH
#endif //#ifdef CDBG_HIGH
#define CDBG_HIGH(fmt, args...) ALOGD_IF(gCamHal3LogLevel >= 1, fmt, ##args)

using namespace android;

namespace qcamera {

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* Time related macros */
typedef int64_t nsecs_t;
#define NSEC_PER_SEC 1000000000LL
#define NSEC_PER_USEC 1000
#define NSEC_PER_33MSEC 33000000LL

extern volatile uint32_t gCamHal3LogLevel;

class QCamera3MetadataChannel;
class QCamera3PicChannel;
class QCamera3HeapMemory;
class QCamera3Exif;

typedef struct {
    camera3_stream_t *stream;
    camera3_stream_buffer_set_t buffer_set;
    stream_status_t status;
    int registered;
    QCamera3Channel *channel;
} stream_info_t;

class QCamera3HardwareInterface {
public:
    /* static variable and functions accessed by camera service */
    static camera3_device_ops_t mCameraOps;
    static int initialize(const struct camera3_device *,
                const camera3_callback_ops_t *callback_ops);
    static int configure_streams(const struct camera3_device *,
                camera3_stream_configuration_t *stream_list);
    static const camera_metadata_t* construct_default_request_settings(
                                const struct camera3_device *, int type);
    static int process_capture_request(const struct camera3_device *,
                                camera3_capture_request_t *request);

    static void dump(const struct camera3_device *, int fd);
    static int flush(const struct camera3_device *);
    static int close_camera_device(struct hw_device_t* device);

public:
    QCamera3HardwareInterface(int cameraId,
            const camera_module_callbacks_t *callbacks);
    virtual ~QCamera3HardwareInterface();
    static void camEvtHandle(uint32_t camera_handle, mm_camera_event_t *evt,
                                          void *user_data);
    int openCamera(struct hw_device_t **hw_device);
    int getMetadata(int type);
    camera_metadata_t* translateCapabilityToMetadata(int type);

    static int getCamInfo(int cameraId, struct camera_info *info);
    static int initCapabilities(int cameraId);
    static int initStaticMetadata(int cameraId);
    static void makeTable(cam_dimension_t* dimTable, uint8_t size, int32_t* sizeTable);
    static void makeFPSTable(cam_fps_range_t* fpsTable, uint8_t size,
                                          int32_t* fpsRangesTable);
    static void makeOverridesList(cam_scene_mode_overrides_t* overridesTable, uint8_t size,
                                   uint8_t* overridesList, uint8_t* supported_indexes, int camera_id);
    static uint8_t filterJpegSizes(int32_t* jpegSizes, int32_t* processedSizes,
                                   uint8_t processedSizesCnt, uint8_t maxCount,
                                   cam_rect_t active_array_size, uint8_t downscale_factor);
    static void convertToRegions(cam_rect_t rect, int32_t* region, int weight);
    static void convertFromRegions(cam_area_t* roi, const camera_metadata_t *settings,
                                   uint32_t tag);
    static bool resetIfNeededROI(cam_area_t* roi, const cam_crop_region_t* scalerCropRegion);
    static void convertLandmarks(cam_face_detection_info_t face, int32_t* landmarks);
    static void postproc_channel_cb_routine(mm_camera_super_buf_t *recvd_frame,
                                            void *userdata);
    static int32_t getScalarFormat(int32_t format);
    static int32_t getSensorSensitivity(int32_t iso_mode);

    static double computeNoiseModelEntryS(int32_t sensitivity);
    static double computeNoiseModelEntryO(int32_t sensitivity);

    static void captureResultCb(mm_camera_super_buf_t *metadata,
                camera3_stream_buffer_t *buffer, uint32_t frame_number,
                void *userdata);

    int initialize(const camera3_callback_ops_t *callback_ops);
    int configureStreams(camera3_stream_configuration_t *stream_list);
    int processCaptureRequest(camera3_capture_request_t *request);
    void dump(int fd);
    int flush();

    int setFrameParameters(camera3_capture_request_t *request,
            cam_stream_ID_t streamID, int blob_request, uint32_t snapshotStreamId);
    int32_t setReprocParameters(camera3_capture_request_t *request,
            metadata_buffer_t *reprocParam, uint32_t snapshotStreamId);
    int translateToHalMetadata(const camera3_capture_request_t *request,
            metadata_buffer_t *parm, uint32_t snapshotStreamId);
    camera_metadata_t* translateCbUrgentMetadataToResultMetadata (
                             metadata_buffer_t *metadata);

    camera_metadata_t* translateFromHalMetadata(metadata_buffer_t *metadata,
                            nsecs_t timestamp, int32_t request_id,
                            const CameraMetadata& jpegMetadata, uint8_t pipeline_depth,
                            uint8_t capture_intent);
    int getJpegSettings(const camera_metadata_t *settings);
    int initParameters();
    void deinitParameters();
    QCamera3ReprocessChannel *addOfflineReprocChannel(const reprocess_config_t &config,
            QCamera3PicChannel *picChHandle, metadata_buffer_t *metadata);
    bool needRotationReprocess();
    bool needReprocess(uint32_t postprocess_mask);
    bool needJpegRotation();
    bool isWNREnabled();
    bool isCACEnabled();
    cam_denoise_process_type_t getWaveletDenoiseProcessPlate();

    void captureResultCb(mm_camera_super_buf_t *metadata,
                camera3_stream_buffer_t *buffer, uint32_t frame_number);

    typedef struct {
        uint32_t fwk_name;
        uint8_t hal_name;
    } QCameraMap;

    typedef struct {
        const char *const desc;
        cam_cds_mode_type_t val;
    } QCameraPropMap;

private:

    int openCamera();
    int closeCamera();
    int AddSetParmEntryToBatch(parm_buffer_t *p_table,
                               cam_intf_parm_type_t paramType,
                               uint32_t paramLength,
                               void *paramValue);
    static int8_t lookupHalName(const QCameraMap arr[],
                      int len, unsigned int fwk_name);
    static int32_t lookupFwkName(const QCameraMap arr[],
                      int len, int hal_name);
    static cam_cds_mode_type_t lookupProp(const QCameraPropMap arr[],
            int len, const char *name);
    static int calcMaxJpegSize(uint8_t camera_id);
    cam_dimension_t getMaxRawSize(uint8_t camera_id);

    int validateCaptureRequest(camera3_capture_request_t *request);
    int validateStreamDimensions(camera3_stream_configuration_t *streamList);
    int validateStreamRotations(camera3_stream_configuration_t *streamList);
    void deriveMinFrameDuration();
    int32_t handlePendingReprocResults(uint32_t frame_number);
    int64_t getMinFrameDuration(const camera3_capture_request_t *request);
    void handleMetadataWithLock(mm_camera_super_buf_t *metadata_buf);
    void handleBufferWithLock(camera3_stream_buffer_t *buffer,
        uint32_t frame_number);
    void unblockRequestIfNecessary();
    void dumpMetadataToFile(tuning_params_t &meta,
                            uint32_t &dumpFrameCount,
                            int32_t enabled,
                            const char *type,
                            uint32_t frameNumber);
    static void getLogLevel();

    void cleanAndSortStreamInfo();
    int queueReprocMetadata(metadata_buffer_t *metadata);
    void extractJpegMetadata(CameraMetadata& jpegMetadata,
            const camera3_capture_request_t *request);
    bool isSupportChannelNeeded(camera3_stream_configuration_t *streamList,
        cam_stream_size_info_t stream_config_info);
public:
    cam_dimension_t calcMaxJpegDim();
    bool needOnlineRotation();
    int getJpegQuality();
    QCamera3Exif *getExifData();
    static void getFlashInfo(const int cameraId,
            bool& hasFlash,
            char (&flashNode)[QCAMERA_MAX_FILEPATH_LENGTH]);

private:
    camera3_device_t   mCameraDevice;
    uint8_t            mCameraId;
    mm_camera_vtbl_t  *mCameraHandle;
    bool               mCameraOpened;
    bool               mCameraInitialized;
    camera_metadata_t *mDefaultMetadata[CAMERA3_TEMPLATE_COUNT];
    int mBlobRequest;

    const camera3_callback_ops_t *mCallbackOps;

    QCamera3MetadataChannel *mMetadataChannel;
    QCamera3PicChannel *mPictureChannel;
    QCamera3RawChannel *mRawChannel;
    QCamera3SupportChannel *mSupportChannel;
    QCamera3RawDumpChannel *mRawDumpChannel;

     //First request yet to be processed after configureStreams
    bool mFirstRequest;
    bool mFlush;
    bool mEnableRawDump;
    QCamera3HeapMemory *mParamHeap;
    metadata_buffer_t* mParameters;
    metadata_buffer_t* mPrevParameters;
    bool m_bWNROn;
    bool m_bIsVideo;
    bool m_bIs4KVideo;
    uint8_t mEisEnable;

    /* Data structure to store pending request */
    typedef struct {
        camera3_stream_t *stream;
        camera3_stream_buffer_t *buffer;
    } RequestedBufferInfo;
    typedef struct {
        uint32_t frame_number;
        uint32_t num_buffers;
        int32_t request_id;
        List<RequestedBufferInfo> buffers;
        int blob_request;
        uint8_t bUrgentReceived;
        nsecs_t timestamp;
        camera3_stream_buffer_t *input_buffer;
        const camera_metadata_t *settings;
        CameraMetadata jpegMetadata;
        uint8_t pipeline_depth;
        uint32_t partial_result_cnt;
        uint8_t capture_intent;
    } PendingRequestInfo;
    typedef struct {
        uint32_t frame_number;
        uint32_t stream_ID;
    } PendingFrameDropInfo;

    // Store the Pending buffers for Flushing
    typedef struct {
        // Frame number pertaining to the buffer
        uint32_t frame_number;
        camera3_stream_t *stream;
        // Buffer handle
        buffer_handle_t *buffer;
    } PendingBufferInfo;

    typedef struct {
        // Total number of buffer requests pending
        uint32_t num_buffers;
        // List of pending buffers
        List<PendingBufferInfo> mPendingBufferList;
        // Last frame number requested
        uint32_t last_frame_number;
    } PendingBuffersMap;

    typedef struct {
        camera3_notify_msg_t notify_msg;
        camera3_stream_buffer_t buffer;
        uint32_t frame_number;
    } PendingReprocessResult;

    typedef KeyedVector<uint32_t, Vector<PendingBufferInfo> > FlushMap;

    List<PendingReprocessResult> mPendingReprocessResultList;
    List<PendingRequestInfo> mPendingRequestsList;
    List<PendingFrameDropInfo> mPendingFrameDropList;
    PendingBuffersMap mPendingBuffersMap;
    pthread_cond_t mRequestCond;
    int mPendingRequest;
    bool mWokenUpByDaemon;
    int32_t mCurrentRequestId;
    camera3_capture_result_t *mLoopBackResult;
    nsecs_t mLoopBackTimestamp;

    //mutex for serialized access to camera3_device_ops_t functions
    pthread_mutex_t mMutex;

    List<stream_info_t*> mStreamInfo;

    int64_t mMinProcessedFrameDuration;
    int64_t mMinJpegFrameDuration;
    int64_t mMinRawFrameDuration;
    power_module_t *m_pPowerModule;   // power module

    uint32_t mMetaFrameCount;
    const camera_module_callbacks_t *mCallbacks;

    uint8_t mCaptureIntent;
    cam_stream_size_info_t mStreamConfigInfo;

    static const QCameraMap EFFECT_MODES_MAP[];
    static const QCameraMap WHITE_BALANCE_MODES_MAP[];
    static const QCameraMap SCENE_MODES_MAP[];
    static const QCameraMap FOCUS_MODES_MAP[];
    static const QCameraMap COLOR_ABERRATION_MAP[];
    static const QCameraMap ANTIBANDING_MODES_MAP[];
    static const QCameraMap AE_FLASH_MODE_MAP[];
    static const QCameraMap FLASH_MODES_MAP[];
    static const QCameraMap FACEDETECT_MODES_MAP[];
    static const QCameraMap FOCUS_CALIBRATION_MAP[];
    static const QCameraMap TEST_PATTERN_MAP[];
    static const QCameraMap REFERENCE_ILLUMINANT_MAP[];
    static const QCameraPropMap CDS_MAP[];
};

}; // namespace qcamera

#endif /* __QCAMERA2HARDWAREINTERFACE_H__ */
