#ifndef ZAD5_GEOMETRY_HPP
#define ZAD5_GEOMETRY_HPP

#include <iostream>
#include <cmath>

#define TO_RADIANS (M_PI / 180)
#define TO_DEGREES (180 / M_PI)

template <class T>
static T to_radians(T degrees) {
    return degrees * TO_RADIANS;
}

template <class T>
static T to_degrees(T radians) {
    return radians * TO_DEGREES;
}

// TODO rename array to scalars

template <class T, size_t N, class R> // as long as used with proper responsibility is safe and bypasses casting problem
class BaseVector {
public:
    BaseVector() = default;
    explicit BaseVector(const T (&array)[N]);

    static R zeros();

    T operator[](size_t i) const;
    T& operator[](size_t i);

    //R& operator=(const BaseVector<T, N, R>& vector);

    template <class _R>
    R operator+(const BaseVector<T, N, _R>& vector) const;

    template <class _R>
    R operator-(const BaseVector<T, N, _R>& vector) const;

    template <class _R>
    T operator*(const BaseVector<T, N, _R>& vector) const;

    template <class _R>
    R& operator+=(const BaseVector<T, N, _R>& vector);

    R operator+(T scalar) const;
    R operator-(T scalar) const;
    R operator*(T scalar) const;
    R operator/(T scalar) const;

    R& operator/=(T scalar);

    template <class _R>
    bool operator==(const BaseVector<T, N, _R>& vector) const;

    template <class _T, size_t _N, class _R>
    friend std::istream& operator>>(std::istream& in, BaseVector<_T, _N, _R>& vector);
    template <class _T, size_t _N, class _R>
    friend std::ostream& operator<<(std::ostream& out, const BaseVector<_T, _N, _R>& vector);

protected:
    T scalars[N];
};
/*
template <class T, size_t N, class R>
BaseVector<T, N, R>::BaseVector(const BaseVector<T, N, R>& vector) {
    for (size_t i = 0; i < N; i++)
        scalars[i] = vector.scalars[i];
}

template <class T, size_t N, class R>
BaseVector<T, N, R>::BaseVector(BaseVector<T, N, R>&& vector) noexcept { // TODO ???
    for (size_t i = 0; i < N; i++)
        scalars[i] = vector.scalars[i];
}
*/
template <class T, size_t N, class R>
BaseVector<T, N, R>::BaseVector(const T (&array)[N]) {
    for (size_t i = 0; i < N; i++)
        scalars[i] = array[i];
}

// TODO make a more general function as scalar and then overload

template <class T, size_t N, class R>
R BaseVector<T, N, R>::zeros() {
    R result;
    for (size_t i = 0; i < N; i++)
        result[i] = 0;
    return result;
}

template <class T, size_t N, class R>
T BaseVector<T, N, R>::operator[](const size_t i) const {
    return scalars[i];
}

template <class T, size_t N, class R>
T& BaseVector<T, N, R>::operator[](const size_t i) {
    return scalars[i];
}
/*
template <class T, size_t N, class R>
BaseVector<T, N, R>& BaseVector<T, N, R>::operator=(const BaseVector<T, N, R>& vector) { // TODO ???
    for (size_t i = 0; i < N; i++)
        scalars[i] = vector.scalars[i];
    return *this;
}
*/
template <class T, size_t N, class R>
template <class _R>
R BaseVector<T, N, R>::operator+(const BaseVector<T, N, _R>& vector) const {
    R result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] + vector.scalars[i];
    return result;
}

template <class T, size_t N, class R>
template <class _R>
R BaseVector<T, N, R>::operator-(const BaseVector<T, N, _R>& vector) const {
    R result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] - vector.scalars[i];
    return result;
}

template <class T, size_t N, class R>
template <class _R>
T BaseVector<T, N, R>::operator*(const BaseVector<T, N, _R> &vector) const {
    T result = 0;
    for (size_t i = 0; i < N; i++)
        result += scalars[i] * vector[i];
    return result;
}

template <class T, size_t N, class R>
template <class _R>
R& BaseVector<T, N, R>::operator+=(const BaseVector<T, N, _R>& vector) {
    for (size_t i = 0; i < N; i++)
        scalars[i] += vector[i];
    return static_cast<R&>(*this); // TODO why?
}

template <class T, size_t N, class R>
R BaseVector<T, N, R>::operator+(T scalar) const {
    BaseVector<T, N, R> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] + scalar;
    return result;
}

template <class T, size_t N, class R>
R BaseVector<T, N, R>::operator-(T scalar) const {
    R result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] - scalar;
    return result;
}

template <class T, size_t N, class R>
R BaseVector<T, N, R>::operator*(T scalar) const {
    R result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] * scalar;
    return result;
}

template <class T, size_t N, class R>
R BaseVector<T, N, R>::operator/(T scalar) const {
    BaseVector<T, N, R> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] / scalar;
    return result;
}

template <class T, size_t N, class R>
R& BaseVector<T, N, R>::operator/=(T scalar) {
    for (size_t i = 0; i < N; i++)
        scalars[i] /= scalar;
    return static_cast<R&>(*this); // TODO why?
}

template <class T, size_t N, class R>
template <class _R>
bool BaseVector<T, N, R>::operator==(const BaseVector<T, N, _R>& vector) const {
    for (size_t i = 0; i < N; i++)
        if (scalars[i] != vector.scalars[i])
            return false;
    return true;
}

template <class T, size_t N, class R>
std::istream& operator>>(std::istream& in, BaseVector<T, N, R>& vector) {
    for (size_t i = 0; i < N; i++)
        in >> vector.scalars[i];
    return in;
}

template <class T, size_t N, class R>
std::ostream& operator<<(std::ostream& out, const BaseVector<T, N, R>& vector) {
    for (size_t i = 0; i < N; i++) {
        out << vector.scalars[i];
        if (i < N - 1)
            out << " ";
    }
    return out;
}

// TODO are those constructors even needed?
template <class T, size_t N>
class Vector : public BaseVector<T, N, Vector<T, N>> {
public:
    Vector() = default;
    explicit Vector(const T (&array)[N]);
};

template <class T, size_t N>
Vector<T, N>::Vector(const T (&array)[N]) : BaseVector<T, N, Vector<T, N>>(array) {}

template <class T>
class Matrix3D;


template <class T>
class Vector3D : public BaseVector<T, 3, Vector3D<T>> {
public:
    Vector3D() = default;
    Vector3D(T x_scalar, T y_scalar, T z_scalar);
    explicit Vector3D(const T (&array)[3]);

    void translate(const Vector3D<T>& translation);

    void rotate(const Vector3D<T>& origin, const Matrix3D<T>& rotation);
    void rotate(const Matrix3D<T>& rotation);

    void rotate_radians(const Vector3D<T>& origin, const Vector3D<T>& radians);
    void rotate_radians(const Vector3D<T>& radians);

    void rotate_degrees(const Vector3D<T>& origin, const Vector3D<T>& degrees);
    void rotate_degrees(const Vector3D<T>& degrees);
};

template <class T> // TODO optimize
Vector3D<T>::Vector3D(T x_scalar, T y_scalar, T z_scalar)
    : BaseVector<T, 3, Vector3D<T>>({x_scalar, y_scalar, z_scalar}) {}

template <class T>
Vector3D<T>::Vector3D(const T (&array)[3]) : BaseVector<T, 3, Vector3D<T>>(array) {}

// TODO correct?

template <class T>
void Vector3D<T>::translate(const Vector3D<T>& translation) {
    *this += translation;
}

template <class T>
void Vector3D<T>::rotate(const Vector3D<T>& origin, const Matrix3D<T>& rotation) {
    *this = rotation * (*this - origin) + origin;
}

template <class T>
void Vector3D<T>::rotate(const Matrix3D<T>& rotation) {
    *this = rotation * (*this);
}

// TODO correct?

template <class T>
void Vector3D<T>::rotate_radians(const Vector3D<T>& origin, const Vector3D<T>& radians) {
    rotate(origin, Matrix3D<T>::rotation_radians(radians));
}

template <class T>
void Vector3D<T>::rotate_radians(const Vector3D<T>& radians) {
    rotate(Matrix3D<T>::rotation_radians(radians));
}

template <class T>
void Vector3D<T>::rotate_degrees(const Vector3D<T>& origin, const Vector3D<T>& degrees) {
    rotate(origin, Matrix3D<T>::rotation_degrees(degrees));
}

template <class T>
void Vector3D<T>::rotate_degrees(const Vector3D<T>& degrees) {
    rotate(Matrix3D<T>::rotation_degrees(degrees));
}

template <class T, size_t N, class R1, class R2>
class BaseMatrix {
public:
    BaseMatrix() = default;
    explicit BaseMatrix(const T (&scalars)[N * N]);

    R2 operator[](size_t x) const;
    R2& operator[](size_t x);

    template <class _R1, class _R2>
    R1 operator*(const BaseMatrix<T, N, _R1, _R2>& matrix) const;

    template <class _R2>
    R2 operator*(const BaseVector<T, N, _R2>& vector) const;

    template <class _R1, class _R2>
    R1& operator*=(const BaseMatrix<T, N, _R1, _R2>& matrix);

    template <class _T, size_t _N, class _R1, class _R2>
    friend std::istream& operator>>(std::istream& in, BaseMatrix<_T, _N, _R1, _R2>& matrix);
    template <class _T, size_t _N, class _R1, class _R2>
    friend std::ostream& operator<<(std::ostream& out, const BaseMatrix<_T, _N, _R1, _R2>& matrix);

protected:
    R2 vectors[N];
};

template <class T, size_t N, class R1, class R2> // TODO could be replaced with Vector(array, size_t offset), but copy constructor might be needed
BaseMatrix<T, N, R1, R2>::BaseMatrix(const T (&scalars)[N * N]) {
    for (size_t x = 0; x < N; x++)
        for (size_t y = 0; y < N; y++)
            vectors[x][y] = scalars[x * N + y];
}

template <class T, size_t N, class R1, class R2>
R2 BaseMatrix<T, N, R1, R2>::operator[](const size_t x) const {
    return vectors[x];
}

template <class T, size_t N, class R1, class R2>
R2& BaseMatrix<T, N, R1, R2>::operator[](const size_t x) {
    return vectors[x];
}

template <class T, size_t N, class R1, class R2>
template <class _R2>
R2 BaseMatrix<T, N, R1, R2>::operator*(const BaseVector<T, N, _R2>& vector) const {
    R2 result;
    for (size_t i = 0; i < N; i++)
        result[i] = vectors[i] * vector;
    return result;
}

template <class T, size_t N, class R1, class R2>
template <class _R1, class _R2>
R1 BaseMatrix<T, N, R1, R2>::operator*(const BaseMatrix<T, N, _R1, _R2>& matrix) const { // TODO check
    R1 result;
    for (size_t x = 0; x < N; x++)
        for (size_t y = 0; y < N; y++)
            result[x][y] = vectors[x] * matrix[y];
    return result;
}

template <class T, size_t N, class R1, class R2>
template <class _R1, class _R2>
R1& BaseMatrix<T, N, R1, R2>::operator*=(const BaseMatrix<T, N, _R1, _R2>& matrix) { // TODO check
    for (size_t x = 0; x < N; x++)
        for (size_t y = 0; y < N; y++)
            vectors[x][y] = vectors[x] * matrix[y];
    return *this;
}

template <class T, size_t N, class R1, class R2>
std::istream& operator>>(std::istream& in, BaseMatrix<T, N, R1, R2>& matrix) {
    for (size_t i = 0; i < N; i++)
        in >> matrix[i];
    return in;
}

template <class T, size_t N, class R1, class R2>
std::ostream& operator<<(std::ostream& out, const BaseMatrix<T, N, R1, R2>& matrix) {
    for (size_t i = 0; i < N; i++) {
        out << matrix[i];
        if (i < N - 1)
            out << "\n";
    }
    return out;
}

template <class T, size_t N>
class Matrix : public BaseMatrix<T, N, Matrix<T, N>, Vector<T, N>> {
public:
    Matrix() = default;
    explicit Matrix(const T (&scalars)[N * N]);
};

template <class T, size_t N>
Matrix<T, N>::Matrix(const T (&scalars)[N * N]) : BaseMatrix<T, N, Matrix<T, N>, Vector<T, N>>(scalars) {}

template <class T>
class Matrix3D : public BaseMatrix<T, 3, Matrix3D<T>, Vector3D<T>> {
public:
    Matrix3D() = default;
    explicit Matrix3D(const T (&scalars)[9]);

    static Matrix3D<T> rotation_radians(T x_radians, T y_radians, T z_radians);
    static Matrix3D<T> rotation_radians(const Vector3D<T>& radians);

    static Matrix3D<T> rotation_degrees(T x_degrees, T y_degrees, T z_degrees);
    static Matrix3D<T> rotation_degrees(const Vector3D<T>& degrees);
};

template <class T>
Matrix3D<T>::Matrix3D(const T (&scalars)[9]) : BaseMatrix<T, 3, Matrix3D<T>, Vector3D<T>>(scalars) {}

template <class T>
Matrix3D<T> Matrix3D<T>::rotation_radians(T x_radians, T y_radians, T z_radians) {
    return Matrix3D<T>({
        cos(y_radians) * cos(z_radians),
        cos(x_radians) * sin(z_radians) + sin(x_radians) * sin(y_radians) * cos(z_radians),
        sin(x_radians) * sin(z_radians) - cos(x_radians) * sin(y_radians) * cos(z_radians),

       -cos(y_radians) * sin(z_radians),
        cos(x_radians) * cos(z_radians) - sin(x_radians) * sin(y_radians) * sin(z_radians),
        sin(x_radians) * cos(z_radians) + cos(x_radians) * sin(y_radians) * sin(z_radians),

        sin(y_radians),
       -sin(x_radians) * cos(y_radians),
        cos(x_radians) * cos(y_radians)
    });
}

template <class T>
Matrix3D<T> Matrix3D<T>::rotation_radians(const Vector3D<T>& radians) {
    return rotation_radians(radians[0], radians[1], radians[2]);
}

template <class T>
Matrix3D<T> Matrix3D<T>::rotation_degrees(T x_degrees, T y_degrees, T z_degrees) {
    return rotation_radians(to_radians(x_degrees), to_radians(y_degrees), to_radians(z_degrees));
}

template <class T>
Matrix3D<T> Matrix3D<T>::rotation_degrees(const Vector3D<T>& degrees) {
    return rotation_degrees(degrees[0], degrees[1], degrees[2]);
}

// TODO find simpler approach

namespace std {
    template <class T, size_t N, class R>
    struct hash<BaseVector<T, N, R>> {
        static constexpr hash<T> hash_t;

        template <class _R> // was _R = R
        size_t operator()(const BaseVector<T, N, _R>& vector) const { // based on: https://stackoverflow.com/a/27216842/8406095
            size_t seed = N;
            for(size_t i = 0; i < N; i++)
                seed ^= hash_t(vector[i]) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
            return seed;
        }
    };

    template <class T, size_t N>
    struct hash<Vector<T, N>> {
        size_t operator()(const Vector<T, N>& vector) const {
            return hash<BaseVector<T, N, Vector<T, N>>>()(vector);
        }
    };

    template <class T>
    struct hash<Vector3D<T>> {
        size_t operator()(const Vector3D<T>& vector) const {
            return hash<BaseVector<T, 3, Vector3D<T>>>()(vector);
        }
    };
}

#endif //ZAD5_GEOMETRY_HPP
