/*
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