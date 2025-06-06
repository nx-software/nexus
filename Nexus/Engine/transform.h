/*
    NX-Software Nexus 3D Engine
    transform.h - Classes for transformation and general vectors
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

#ifdef WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllexport)
#endif
#else
#define ENGINE_API
#endif

namespace Transform{
    class ENGINE_API Vector2{
        public:
            // The vars are public becuase Vector2::GetX sounds stupid
            float x, y;
            Vector2();
            Vector2(float, float);

            // Operators
            Vector2 operator+(Vector2 other);
    };

    class ENGINE_API Vector3{
        public:
            // The vars are public becuase Vector2::GetX sounds stupid
            float x, y, z;
            Vector3();
            Vector3(float, float, float);

            // Operators
            Vector3 operator+(Vector3 other);
    };

    class ENGINE_API Transform{
        public:
            Vector3 position, scale;
            float angle;
    
            // Empty
            Transform();
            // Individial Params
            Transform(float, float, float, float);
            // Nah
            Transform(Vector3, float);
    };
}