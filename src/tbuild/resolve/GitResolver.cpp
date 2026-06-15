/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */
#include "tbuild/resolve/GitResolver.hpp"

#include <git2.h>

namespace toolkit
{

    bool GitResolver::available() const
    {
        return git_libgit2_features() >= 0;
    }

} // namespace toolkit
