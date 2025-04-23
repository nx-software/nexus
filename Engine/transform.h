/*
*/
#pragma once
namespace Transform{
    class Vector2{
        public:
            // The vars are public becuase Vector2::GetX sounds stupid
            float x, y;
            Vector2();
            Vector2(float, float);

            // Operators
            Vector2 operator+(Vector2 other);
    };

    class Vector3{
        public:
            // The vars are public becuase Vector2::GetX sounds stupid
            float x, y, z;
            Vector3();
            Vector3(float, float, float);

            // Operators
            Vector3 operator+(Vector3 other);
    };

    class Transform{
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