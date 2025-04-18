#ifndef CORETUILS_HPP
#define CORETUILS_HPP

#include <QString>

namespace coreUtils
{
    QString makeCacheFileName(const QString& argName) noexcept;
    bool shouldRefreshCache(const QString& argName) noexcept;
}



#endif // CORETUILS_HPP