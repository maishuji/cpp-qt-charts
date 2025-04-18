#include "CoreUtils.hpp"

#include <QDateTime>
#include <QDir>
#include <QFile>

namespace coreUtils {
QString makeCacheFileName(const QString& argName) noexcept {
    // Get the home directory path
    QString homeDir = QDir::homePath();
    QString timestamp =
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString cacheFileName = homeDir + "/.cache/" + argName + "_" + timestamp;
    return cacheFileName;
}
}  // namespace CoreUtils