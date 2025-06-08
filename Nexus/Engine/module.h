/*
    NX-Software Nexus 3D Engine
    module.h - Code for accessing optional modules, such as Audio and Input
    Copyright (C) 2025 Electro-Corp

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <string>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "../CrashHandler/CrashHandler.h"

namespace Nexus {
    class Module {
    private:
#ifdef _WIN32
        HINSTANCE library;
#endif
    public:
        Module(std::string name);
    };
}