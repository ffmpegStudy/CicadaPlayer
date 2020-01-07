//
// Created by lifujun on 2019/5/30.
//

#define  LOG_TAG "CacheModule"

#include <utils/file/FileUtils.h>
#include <utils/af_string.h>
#include "CacheModule.h"
#include <utility>
#include <dirent.h>
#include "utils/frame_work_log.h"

#define TMP_SUFFIX (".tmp")

using namespace Cicada;

CacheModule::CacheModule()
{
}

CacheModule::~CacheModule()
{
    stop();
}

void CacheModule::setCacheConfig(const CacheConfig &config)
{
    AF_LOGD("---> setCacheConfig()");
    mCacheChecker.setCacheConfig(config);
    mCachePath.setCacheConfig(config);
}

void CacheModule::setSourceUrl(const string &URL)
{
    AF_LOGD("---> setOriginalPlayInfo() url = %s", URL.c_str());
    mCachePath.setSourceURL(URL);
}

void CacheModule::setDescription(const string &description)
{
    AF_LOGD("---> setDescription() description = %s", description.c_str());
    mDescription = description;
}

string CacheModule::getSourceUrl()
{
    return mCachePath.getSourceUrl();;
}

string CacheModule::getCachedFilePath()
{
    return mCachePath.getCachedFilePath();
}

CacheRet CacheModule::checkCanBeCached(const string &acturalPlayURL)
{
    AF_LOGD("---> prepare()");
    {
        unique_lock<mutex> lock(mStatusMutex);

        if (mStatus != Stopped) {
            return CACHE_ERROR_STATUS;
        }

        if (AfString::isLocalURL(acturalPlayURL)) {
            mStatus = Status::Stopped;
            AF_LOGE("url is local source");
            return CACHE_ERROR_LOCAL_SOURCE;
        }

        const CacheRet &cacheRet = mCacheChecker.checkConfig();
        mStatus = (cacheRet.mCode == CACHE_SUCCESS.mCode ? Status::Prepared : Status::Stopped);
        return cacheRet;
    }
}

void CacheModule::setMediaInfo(int64_t fileSize, int64_t duration)
{
    AF_LOGD("---> setMediaInfo()");
    mMediaInfoSet = true;
    mCacheChecker.setMediaInfo(fileSize, duration);
}

bool CacheModule::isMediaInfoSet()
{
    return mMediaInfoSet;
}

void CacheModule::setMetaCallback(function<bool(StreamType, Stream_meta *)> metaCallback)
{
    mMetaCallback = std::move(metaCallback);
}

void CacheModule::setErrorCallback(function<void(int, string)> callback)
{
    mErrorCallback = std::move(callback);
}

CacheRet CacheModule::start()
{
    {
        unique_lock<mutex> lock(mStatusMutex);

        if (mStatus == Status::Stopped) {
            AF_LOGE("---> start()  , mStatus == Status::Stopped return ");
            return CACHE_ERROR_STATUS;
        }
    }
    AF_LOGD("---> start()");
    const CacheRet &canCheck = checkCanCache();

    if (canCheck.mCode != CACHE_SUCCESS.mCode) {
        AF_LOGE("---> start()  , checkCanCache fail.. return ");
        return canCheck;
    }

    {
        std::unique_lock<mutex> remuxerLock(mReumxerMutex);

        if (mCacheFileRemuxer != nullptr) {
            mCacheFileRemuxer->interrupt();
            mCacheFileRemuxer->stop();
            delete mCacheFileRemuxer;
            mCacheFileRemuxer = nullptr;
        }

        const string &cacheFilePath = mCachePath.getCachePath();
        string       cacheTmpPath   = cacheFilePath + TMP_SUFFIX;
        mCacheFileRemuxer          = new CacheFileRemuxer(cacheTmpPath, mDescription);
        mCacheFileRemuxer->setMetaCallback(mMetaCallback);
        mCacheFileRemuxer->setErrorCallback([this](int code, string msg) -> void {
            if (mErrorCallback != nullptr)
            {
                mErrorCallback(code, msg);
            }
        });
        bool         prepareSucced = mCacheFileRemuxer->prepare();

        if (!prepareSucced) {
            AF_LOGE("---> start()  , cacheFileRemuxer->prepare() fail");
            delete mCacheFileRemuxer;
            mCacheFileRemuxer = nullptr;
            return CACHE_ERROR_FILE_REMUXER_OPEN_ERROR;
        }

        AF_LOGD("---> start()  , cacheFileRemuxer->start()");
        mCacheFileRemuxer->start();
    }

    return CACHE_SUCCESS;
}

CacheRet CacheModule::checkCanCache()
{
    AF_LOGD("---> checkCanCache()");
    bool matchConfig = mCacheChecker.checkMediaInfo();

    if (!matchConfig) {
        // not match with config
        AF_LOGE("---> checkCanCache() not match with config");
        return CACHE_ERROR_MEDIA_INFO_NOT_MATCH;
    }

    bool spaceEnough = mCacheChecker.checkSpaceEnough();

    if (!spaceEnough) {
        AF_LOGE("---> checkCanCache() space not Enough");
        return CACHE_ERROR_MEDIA_INFO_NOT_MATCH;
    }

    return CACHE_SUCCESS;
}


void CacheModule::addFrame(unique_ptr<IAFPacket> frame, StreamType type)
{
    std::unique_lock<mutex> lock(mReumxerMutex);

    if (mCacheFileRemuxer != nullptr) {
        mCacheFileRemuxer->addFrame(std::move(frame), type);
    }
}

void CacheModule::streamEnd()
{
    AF_LOGD("---> streamEnd()");
    mEos = true;
    stop();
}

void CacheModule::stop()
{
    {
        unique_lock<mutex> lock(mStatusMutex);
        mStatus = Status::Stopped;
    }
    AF_LOGD("---> stop()");
    {
        std::unique_lock<mutex> remuxerLock(mReumxerMutex);
        mCacheRet = CacheStatus::idle;

        if (mCacheFileRemuxer != nullptr) {
            mCacheFileRemuxer->interrupt();
            mCacheFileRemuxer->stop();
            delete mCacheFileRemuxer;
            mCacheFileRemuxer = nullptr;
            const string &cachePath   = mCachePath.getCachePath();
            string       cacheTmpPath = cachePath + TMP_SUFFIX;

            if (mEos) {
                // completion , rename file
                FileUtils::Rename(cacheTmpPath.c_str(), cachePath.c_str());
                mCacheRet = CacheStatus::success;
            } else {
                // not completion ,  need delete cached file
                FileUtils::rmrf(cacheTmpPath.c_str());
                mCacheRet = CacheStatus::fail;
            }
        }
    }
}

void CacheModule::reset()
{
    AF_LOGD("---> reset()");
    unique_lock<mutex> lock(mStatusMutex);
    mEos            = false;
    mMediaInfoSet   = false;
    mCacheRet       = CacheStatus::idle;;
    mCacheChecker.reset();
    mCachePath.reset();
}

CacheModule::CacheStatus CacheModule::getCacheStatus()
{
    AF_LOGD("<---- getCacheStatus() %d", mCacheRet);
    return mCacheRet;
}

