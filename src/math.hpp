#pragma once
#include <cmath>

struct vec3
{
    float x, y, z;

    /* Operators */
    vec3 operator+( const vec3& v ) const
    {
        return { x + v.x, y + v.y, z + v.z };
    }

    vec3 operator-( const vec3& v ) const
    {
        return { x - v.x, y - v.y, z - v.z };
    }

    vec3 operator*( const vec3& v ) const
    {
        return { x * v.x, y * v.y, z * v.z };
    }

    vec3 operator/( const vec3& v ) const
    {
        return { x / v.x, y / v.y, z / v.z };
    }

    vec3 operator+( float f ) const
    {
        return { x + f, y + f, z + f };
    }

    vec3 operator-( float f ) const
    {
        return { x - f, y - f, z - f };
    }

    vec3 operator*( float f ) const
    {
        return { x * f, y * f, z * f };
    }

    vec3 operator/( float f ) const
    {
        return { x / f, y / f, z / f };
    }

    vec3& operator+=( const vec3& v )
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    vec3& operator-=( const vec3& v )
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }

    vec3& operator*=( const vec3& v )
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;

        return *this;
    }

    vec3& operator/=( const vec3& v )
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;

        return *this;
    }

    vec3& operator+=( float f )
    {
        x += f;
        y += f;
        z += f;

        return *this;
    }

    vec3& operator-=( float f )
    {
        x -= f;
        y -= f;
        z -= f;

        return *this;
    }

    vec3& operator*=( float f )
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

    vec3& operator/=( float f )
    {
        x /= f;
        y /= f;
        z /= f;

        return *this;
    }

    vec3 operator-() const
    {
        return { -x, -y, -z };
    }

    /* Functions */
    float length() const
    {
        return sqrt( x * x + y * y + z * z );
    }

    vec3 normalize() const
    {
        return *this / length();
    }

    float dot( const vec3& v ) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    vec3 cross( const vec3& v ) const
    {
        return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
    }

    vec3 reflect( const vec3& normal ) const
    {
        return *this - normal * 2.0f * dot( normal );
    }

    vec3 refract( const vec3& normal, float eta ) const
    {
        float k = 1.0f - eta * eta * ( 1.0f - dot( normal ) * dot( normal ) );

        if ( k < 0.0f )
        {
            return { 0.0f, 0.0f, 0.0f };
        }

        return *this * eta - normal * ( eta * dot( normal ) + sqrt( k ) );
    }

    /* Static functions */
    static vec3 lerp( const vec3& a, const vec3& b, float t )
    {
        return a + ( b - a ) * t;
    }

    static vec3 random()
    {
        return { rand() / ( float )RAND_MAX, rand() / ( float )RAND_MAX, rand() / ( float )RAND_MAX };
    }

    static vec3 random( float min, float max )
    {
        return { min + ( max - min ) * rand() / ( float )RAND_MAX, min + ( max - min ) * rand() / ( float )RAND_MAX, min + ( max - min ) * rand() / ( float )RAND_MAX };
    }

    static vec3 random_in_unit_sphere()
    {
        while ( true )
        {
            vec3 p = random( -1.0f, 1.0f );

            if ( p.length() < 1.0f )
            {
                return p;
            }
        }
    }

    static vec3 random_unit_vector()
    {
        return random_in_unit_sphere().normalize();
    }

    static vec3 random_in_hemisphere( const vec3& normal )
    {
        vec3 in_unit_sphere = random_in_unit_sphere();

        if ( in_unit_sphere.dot( normal ) > 0.0f )
        {
            return in_unit_sphere;
        }

        return -in_unit_sphere;
    }

    static vec3 random_in_unit_disk()
    {
        while ( true )
        {
            vec3 p = { random( -1.0f, 1.0f ).x, random( -1.0f, 1.0f ).y, 0.0f };

            if ( p.length() < 1.0f )
            {
                return p;
            }
        }
    }

    static vec3 random_cosine_direction()
    {
        float r1 = rand() / ( float )RAND_MAX;
        float r2 = rand() / ( float )RAND_MAX;
        float z = sqrt( 1.0f - r2 );
        float phi = 2.0f * 3.14159265359f * r1;
        float x = cos( phi ) * sqrt( r2 );
        float y = sin( phi ) * sqrt( r2 );

        return { x, y, z };
    }
};

struct quat
{
    float x, y, z, w;

    /* Operators */
    quat operator+( const quat& q ) const
    {
        return { x + q.x, y + q.y, z + q.z, w + q.w };
    }

    quat operator-( const quat& q ) const
    {
        return { x - q.x, y - q.y, z - q.z, w - q.w };
    }

    quat operator*( const quat& q ) const
    {
        return { w * q.x + x * q.w + y * q.z - z * q.y,
                 w * q.y + y * q.w + z * q.x - x * q.z,
                 w * q.z + z * q.w + x * q.y - y * q.x,
                 w * q.w - x * q.x - y * q.y - z * q.z };
    }

    quat operator/( const quat& q ) const
    {
        return *this * q.inverse();
    }

    quat operator+( float f ) const
    {
        return { x + f, y + f, z + f, w + f };
    }

    quat operator-( float f ) const
    {
        return { x - f, y - f, z - f, w - f };
    }

    quat operator*( float f ) const
    {
        return { x * f, y * f, z * f, w * f };
    }

    quat operator/( float f ) const
    {
        return { x / f, y / f, z / f, w / f };
    }

    quat& operator+=( const quat& q )
    {
        x += q.x;
        y += q.y;
        z += q.z;
        w += q.w;

        return *this;
    }

    quat& operator-=( const quat& q )
    {
        x -= q.x;
        y -= q.y;
        z -= q.z;
        w -= q.w;

        return *this;
    }

    quat& operator*=( const quat& q )
    {
        *this = *this * q;

        return *this;
    }

    quat& operator/=( const quat& q )
    {
        *this = *this / q;

        return *this;
    }

    quat& operator+=( float f )
    {
        x += f;
        y += f;
        z += f;
        w += f;

        return *this;
    }

    quat& operator-=( float f )
    {
        x -= f;
        y -= f;
        z -= f;
        w -= f;

        return *this;
    }

    quat& operator*=( float f )
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;

        return *this;
    }

    quat& operator/=( float f )
    {
        x /= f;
        y /= f;
        z /= f;
        w /= f;

        return *this;
    }

    quat operator-() const
    {
        return { -x, -y, -z, -w };
    }

    /* Functions */
    float length() const
    {
        return sqrt( x * x + y * y + z * z + w * w );
    }

    quat normalize() const
    {
        return *this / length();
    }

    quat conjugate() const
    {
        return { -x, -y, -z, w };
    }

    quat inverse() const
    {
        return conjugate() / ( x * x + y * y + z * z + w * w );
    }

    vec3 rotate( const vec3& v ) const
    {
        quat p = { v.x, v.y, v.z, 0.0f };
        quat q = *this * p * conjugate();

        return { q.x, q.y, q.z };
    }

    /* Static functions */
    static quat lerp( const quat& a, const quat& b, float t )
    {
        return a + ( b - a ) * t;
    }

    static quat slerp( const quat& a, const quat& b, float t )
    {
        float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

        if ( dot < 0.0f )
        {
            dot = -dot;
            return ( a - b ) * t + b;
        }

        if ( dot > 0.9995f )
        {
            return lerp( a, b, t );
        }

        float theta_0 = acos( dot );
        float theta = theta_0 * t;
        float sin_theta = sin( theta );
        float sin_theta_0 = sin( theta_0 );

        float s0 = cos( theta ) - dot * sin_theta / sin_theta_0;
        float s1 = sin_theta / sin_theta_0;

        return a * s0 + b * s1;
    }
};