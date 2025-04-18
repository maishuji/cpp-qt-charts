#include "CoreUtils.hpp"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace coreUtils {
QString makeCacheFileName(const QString& argName) noexcept {
    // Get the home directory path
    QString homeDir = QDir::homePath();
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString cacheFileName = homeDir + "/.cache/" + argName + "_" + timestamp;
    return cacheFileName;
}

bool shouldRefreshCache(const QString& argName) noexcept {
    // Check if the cache file exists
    QString cacheFileName = makeCacheFileName(argName);
    QFileInfo cacheFile(cacheFileName);
    if (cacheFile.exists()) {
        // Check the last modified time of the cache file
        QDateTime lastModified = cacheFile.lastModified();
        QDateTime currentTime = QDateTime::currentDateTime();
        QDateTime oneHourAgo = QDateTime::currentDateTime().addSecs(-3600);
        if (lastModified > oneHourAgo) {
            return false;  // Cache is still valid
        }
    }
    return true;  // Cache file doesn't exist, so refresh is needed
}
}  // namespace coreUtils