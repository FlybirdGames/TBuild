/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved.
 */

#include "tbuild/config/TomlConfig.hpp"

#include "tbuild/core/FileSystem.hpp"

#include <toml++/toml.hpp>

namespace toolkit
{

    std::optional<ToolConfig> ToolConfig::load(const std::filesystem::path &path)
    {
        if (!File::exists(path))
        {
            return std::nullopt;
        }

        auto table = toml::parse_file(path.string());
        ToolConfig config;
        config.buildDir = table["build_dir"].value_or(config.buildDir);
        config.defaultPlatform = table["default_platform"].value_or("");
        config.defaultConfig = table["default_config"].value_or("");
        config.defaultArch = table["default_arch"].value_or("");
        return config;
    }

} // namespace toolkit
