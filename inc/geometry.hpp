#ifndef ZAD5_GEOMETRY_HPP
#define ZAD5_GEOMETRY_HPP

#include <iostream>
#include <cmath>

#define TO_RADIANS (M_PI / 180)
#define TO_DEGREES (180 / M_PI)

namespace geometry {
    template <class T>
    static T to_radians(T degrees) {
        return degrees * TO_RADIANS;
    }

    template <class T>
    static T to_degrees(T radians) {
        return radians * TO_DEGREES;
    }

    template <class T, size_t N, class R>
    class basic_vector {
    public:
        basic_vector() = default;
        explicit basic_vector(const T (&scalars)[N]) noexcept;
        explicit basic_vector(T scalar) noexcept;

        static R zeros() noexcept;

        const T& operator[](size_t i) const;
        T& operator[](size_t i);

        template <class _R>
        T operator*(const basic_vector<T, N, _R>& vector) const;

        template <class _R>
        R operator+(const basic_vector<T, N, _R>& vector) const;

        template <class _R>
        R operator-(const basic_vector<T, N, _R>& vector) const;

        template <class _R>
        R& operator+=(const basic_vector<T, N, _R>& vector);

        template <class _R>
        R& operator-=(const basic_vector<T, N, _R>& vector);

        R operator+(T scalar) const;
        R operator-(T scalar) const;
        R operator*(T scalar) const;
        R operator/(T scalar) const;
        R operator%(T scalar) const;

        R& operator+=(T scalar);
        R& operator-=(T scalar);
        R& operator*=(T scalar);
        R& operator/=(T scalar);
        R& operator%=(T scalar);

        template <class _R>
        bool operator==(const basic_vector<T, N, _R>& vector) const;

        template <class _T, size_t _N, class _R>
        friend std::istream& operator>>(std::istream& in, basic_vector<_T, _N, _R>& vector);

        template <class _T, size_t _N, class _R>
        friend std::ostream& operator<<(std::ostream& out, const basic_vector<_T, _N, _R>& vector);

    protected:
        template <template <class> class F, class _R>
        R copy_operator_vector(const basic_vector<T, N, _R>& vector) const;

        template <template <class> class F, class _R>
        R& assign_operator_vector(const basic_vector<T, N, _R>& vector);

        template <template <class> class F>
        R copy_operator_scalar(T scalar) const;

        template <template <class> class F>
        R& assign_operator_scalar(T scalar);

        T scalars[N];
    };

    template <class T, size_t N, class R>
    template <template <class> class F, class _R>
    R basic_vector<T, N, R>::copy_operator_vector(const basic_vector<T, N, _R>& vector) const {
        static constexpr auto operand = F<T>();

        R result;
        for (size_t i = 0; i < N; i++)
            result.scalars[i] = operand(scalars[i], vector.scalars[i]);
        return result;
    }

    template <class T, size_t N, class R>
    template <template <class> class F, class _R>
    R& basic_vector<T, N, R>::assign_operator_vector(const basic_vector<T, N, _R>& vector) {
        static constexpr auto operand = F<T>();

        for (size_t i = 0; i < N; i++)
            scalars[i] = operand(scalars[i], vector.scalars[i]);
        return static_cast<R&>(*this);
    }

    template <class T, size_t N, class R>
    template <template <class> class F>
    R basic_vector<T, N, R>::copy_operator_scalar(T scalar) const {
        static constexpr auto operand = F<T>();

        R result;
        for (size_t i = 0; i < N; i++)
            result.scalars[i] = operand(scalars[i], scalar);
        return result;
    }

    template <class T, size_t N, class R>
    template <template <class> class F>
    R& basic_vector<T, N, R>::assign_operator_scalar(T scalar) {
        static constexpr auto operand = F<T>();

        for (size_t i = 0; i < N; i++)
            scalars[i] = operand(scalars[i], scalar);
        return static_cast<R&>(*this);
    }


    template <class T, size_t N, class R>
    basic_vector<T, N, R>::basic_vector(const T (&scalars)[N]) noexcept {
        for (size_t i = 0; i < N; i++)
            this->scalars[i] = scalars[i];
    }

    template <class T, size_t N, class R>
    basic_vector<T, N, R>::basic_vector(T scalar) noexcept {
        for (size_t i = 0; i < N; i++)
            this->scalars[i] = scalar;
    }

    template <class T, size_t N, class R>
    R basic_vector<T, N, R>::zeros() noexcept {
        return R(0);
    }

    template <class T, size_t N, class R>
    const T& basic_vector<T, N, R>::operator[](const size_t i) const {
        return scalars[i];
    }

    template <class T, size_t N, class R>
    T& basic_vector<T, N, R>::operator[](const size_t i) {
        return scalars[i];
    }

    template <class T, size_t N, class R>
    template <class _R>
    T basic_vector<T, N, R>::operator*(const basic_vector<T, N, _R>& vector) const {
        T result = 0;
        for (size_t i = 0; i < N; i++)
            result += scalars[i] * vector.scalars[i];
        return result;
    }

    template <class T, size_t N, class R>
    template <class _R>
    R basic_vector<T, N, R>::operator+(const basic_vector<T, N, _R>& vector) const {
        return copy_operator_vector<std::plus, _R>(vector);
    }

    template <class T, size_t N, class R>
    template <class _R>
    R basic_vector<T, N, R>::operator-(const basic_vector<T, N, _R>& vector) const {
        return copy_operator_vector<std::minus, _R>(vector);
    }

    template <class T, size_t N, class R>
    template <class _R>
    R& basic_vector<T, N, R>::operator+=(const basic_vector<T, N, _R>& vector) {
        return assign_operator_vector<std::plus, _R>(vector);
    }

    template <class T, size_t N, class R>
    template <class _R>
    R& basic_vector<T, N, R>::operator-=(const basic_vector<T, N, _R>& vector) {
        return assign_operator_vector<std::minus, _R>(vector);
    }

    template <class T, size_t N, class R>
    R basic_vector<T, N, R>::operator+(T scalar) const {
        return copy_operator_scalar<std::plus>(scalar);
    }

    template <class T, size_t N, class R>
    R basic_vector<T, N, R>::operator-(T scalar) const {
        return copy_operator_scalar<std::minus>(scalar);
    }

    template <class T, size_t N, class R>
    R basic_vector<T, N, R>::operator*(T scalar) const {
        return copy_operator_scalar<std::multiplies>(scalar);
    }

    template <class T, size_t N, class R>
    R basic_vector<T, N, R>::operator/(T scalar) const {
        return copy_operator_scalar<std::divides>(scalar);
    }

    template <class T, size_t N, class R>
    R basic_vector<T, N, R>::operator%(T scalar) const {
        return copy_operator_scalar<std::modulus>(scalar);
    }

    template <class T, size_t N, class R>
    R& basic_vector<T, N, R>::operator+=(T scalar) {
        return assign_operator_scalar<std::plus>(scalar);
    }

    template <class T, size_t N, class R>
    R& basic_vector<T, N, R>::operator-=(T scalar) {
        return assign_operator_scalar<std::minus>(scalar);
    }

    template <class T, size_t N, class R>
    R& basic_vector<T, N, R>::operator*=(T scalar) {
        return assign_operator_scalar<std::multiplies>(scalar);
    }

    template <class T, size_t N, class R>
    R& basic_vector<T, N, R>::operator/=(T scalar) {
        return assign_operator_scalar<std::divides>(scalar);
    }

    template <class T, size_t N, class R>
    R& basic_vector<T, N, R>::operator%=(T scalar) {
        return assign_operator_scalar<std::modulus>(scalar);
    }

    template <class T, size_t N, class R>
    template <class _R>
    bool basic_vector<T, N, R>::operator==(const basic_vector<T, N, _R>& vector) const {
        for (size_t i = 0; i < N; i++)
            if (scalars[i] != vector.scalars[i])
                return false;
        return true;
    }

    template <class T, size_t N, class R>
    std::istream& operator>>(std::istream& in, basic_vector<T, N, R>& vector) {
        for (size_t i = 0; i < N; i++)
            in >> vector.scalars[i];
        return in;
    }

    template <class T, size_t N, class R>
    std::ostream& operator<<(std::ostream& out, const basic_vector<T, N, R>& vector) {
        for (size_t i = 0; i < N; i++) {
            out << vector.scalars[i];
            if (i < N - 1)
                out << " ";
        }
        return out;
    }

    template <class T, size_t N>
    class vector : public basic_vector<T, N, vector<T, N>> {
    public:
        vector() = default;
        explicit vector(const T (&scalar)[N]) noexcept;
        explicit vector(T scalar) noexcept;
    };

    template <class T, size_t N>
    vector<T, N>::vector(const T (&scalars)[N]) noexcept
        : basic_vector<T, N, vector<T, N>>(scalars) {}

    template <class T, size_t N>
    vector<T, N>::vector(T scalar) noexcept
        : basic_vector<T, N, vector<T, N>>(scalar) {}

    template <class T>
    class matrix3d;

    template <class T>
    class vector3d : public basic_vector<T, 3, vector3d<T>> {
    public:
        vector3d() = default;
        vector3d(T x_scalar, T y_scalar, T z_scalar);
        explicit vector3d(const T (&scalars)[3]) noexcept;
        explicit vector3d(T scalar) noexcept;

        void translate(const vector3d<T>& translation);

        void rotate(const vector3d<T>& point, const matrix3d<T>& rotation);
        void rotate(const matrix3d<T>& rotation);

        void rotate_radians(const vector3d<T>& point, const vector3d<T>& radians);
        void rotate_radians(const vector3d<T>& radians);

        void rotate_degrees(const vector3d<T>& point, const vector3d<T>& degrees);
        void rotate_degrees(const vector3d<T>& degrees);
    };

    template <class T>
    vector3d<T>::vector3d(T x_scalar, T y_scalar, T z_scalar)
        : basic_vector<T, 3, vector3d<T>>({x_scalar, y_scalar, z_scalar}) {}

    template <class T>
    vector3d<T>::vector3d(const T (&scalars)[3]) noexcept
        : basic_vector<T, 3, vector3d<T>>(scalars) {}

    template <class T>
    vector3d<T>::vector3d(T scalar) noexcept
        : basic_vector<T, 3, vector3d<T>>(scalar) {}

    template <class T>
    void vector3d<T>::translate(const vector3d<T>& translation) {
        this->operator+=(translation);
    }

    template <class T>
    void vector3d<T>::rotate(const vector3d<T>& point, const matrix3d<T>& rotation) {
        this->operator=(rotation * (*this - point) + point);
    }

    template <class T>
    void vector3d<T>::rotate(const matrix3d<T>& rotation) {
        this->operator=(rotation * (*this));
    }

    template <class T>
    void vector3d<T>::rotate_radians(const vector3d<T>& point, const vector3d<T>& radians) {
        rotate(point, matrix3d<T>::rotation_radians(radians));
    }

    template <class T>
    void vector3d<T>::rotate_radians(const vector3d<T>& radians) {
        rotate(matrix3d<T>::rotation_radians(radians));
    }

    template <class T>
    void vector3d<T>::rotate_degrees(const vector3d<T>& point, const vector3d<T>& degrees) {
        rotate(point, matrix3d<T>::rotation_degrees(degrees));
    }

    template <class T>
    void vector3d<T>::rotate_degrees(const vector3d<T>& degrees) {
        rotate(matrix3d<T>::rotation_degrees(degrees));
    }

    template <class T, size_t N, class R1, class R2>
    class basic_matrix {
    public:
        basic_matrix() = default;
        explicit basic_matrix(const T (&scalars)[N * N]) noexcept;
        explicit basic_matrix(T scalar) noexcept;

        static R1 zeros() noexcept;

        const R2& operator[](size_t x) const;
        R2& operator[](size_t x);

        template <class _R1, class _R2>
        R1 operator*(const basic_matrix<T, N, _R1, _R2>& matrix) const;

        template <class _R1, class _R2>
        R1& operator*=(const basic_matrix<T, N, _R1, _R2>& matrix);

        template <class _R2>
        R2 operator*(const basic_vector<T, N, _R2>& vector) const;

        template <class _R1, class _R2>
        R1 operator+(const basic_matrix<T, N, _R1, _R2>& matrix) const;

        template <class _R1, class _R2>
        R1 operator-(const basic_matrix<T, N, _R1, _R2>& matrix) const;

        template <class _R1, class _R2>
        R1& operator+=(const basic_matrix<T, N, _R1, _R2>& matrix);

        template <class _R1, class _R2>
        R1& operator-=(const basic_matrix<T, N, _R1, _R2>& matrix);

        R1 operator+(T scalar) const;
        R1 operator-(T scalar) const;
        R1 operator*(T scalar) const;
        R1 operator/(T scalar) const;
        R1 operator%(T scalar) const;

        R1& operator+=(T scalar);
        R1& operator-=(T scalar);
        R1& operator*=(T scalar);
        R1& operator/=(T scalar);
        R1& operator%=(T scalar);

        template <class _T, size_t _N, class _R1, class _R2>
        friend std::istream& operator>>(std::istream& in, basic_matrix<_T, _N, _R1, _R2>& matrix);

        template <class _T, size_t _N, class _R1, class _R2>
        friend std::ostream& operator<<(std::ostream& out, const basic_matrix<_T, _N, _R1, _R2>& matrix);

    protected:
        template <template <class> class F, class _R1, class _R2>
        R1 copy_operator_matrix(const basic_matrix<T, N, _R1, _R2>& matrix) const;

        template <template <class> class F, class _R1, class _R2>
        R1& assign_operator_matrix(const basic_matrix<T, N, _R1, _R2>& matrix);

        template <template <class> class F>
        R1 copy_operator_scalar(T scalar) const;

        template <template <class> class F>
        R1& assign_operator_scalar(T scalar);

        R2 vectors[N];
    };

    template <class T, size_t N, class R1, class R2>
    template <template <class> class F, class _R1, class _R2>
    R1 basic_matrix<T, N, R1, R2>::copy_operator_matrix(const basic_matrix<T, N, _R1, _R2>& matrix) const {
        static constexpr auto operand = F<T>();

        R1 result;
        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                result.vectors[x][y] = operand(vectors[x][y], matrix.vectors[x][y]);
        return result;
    }

    template <class T, size_t N, class R1, class R2>
    template <template <class> class F, class _R1, class _R2>
    R1& basic_matrix<T, N, R1, R2>::assign_operator_matrix(const basic_matrix<T, N, _R1, _R2>& matrix) {
        static constexpr auto operand = F<T>();

        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                vectors[x][y] = operand(vectors[x][y], matrix.vectors[x][y]);
        return static_cast<R1&>(*this);
    }

    template <class T, size_t N, class R1, class R2>
    template <template <class> class F>
    R1 basic_matrix<T, N, R1, R2>::copy_operator_scalar(T scalar) const {
        static constexpr auto operand = F<T>();

        R1 result;
        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                result.vectors[x][y] = operand(vectors[x][y], scalar);
        return result;
    }

    template <class T, size_t N, class R1, class R2>
    template <template <class> class F>
    R1& basic_matrix<T, N, R1, R2>::assign_operator_scalar(T scalar) {
        static constexpr auto operand = F<T>();

        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                vectors[x][y] = operand(vectors[x][y], scalar);
        return static_cast<R1&>(*this);
    }

    template <class T, size_t N, class R1, class R2>
    basic_matrix<T, N, R1, R2>::basic_matrix(const T (&scalars)[N * N]) noexcept {
        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                vectors[x][y] = scalars[x * N + y];
    }

    template <class T, size_t N, class R1, class R2>
    basic_matrix<T, N, R1, R2>::basic_matrix(T scalar) noexcept {
        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                vectors[x][y] = scalar;
    };

    template <class T, size_t N, class R1, class R2>
    R1 basic_matrix<T, N, R1, R2>::zeros() noexcept {
        return R1(0);
    }

    template <class T, size_t N, class R1, class R2>
    const R2& basic_matrix<T, N, R1, R2>::operator[](const size_t x) const {
        return vectors[x];
    }

    template <class T, size_t N, class R1, class R2>
    R2& basic_matrix<T, N, R1, R2>::operator[](const size_t x) {
        return vectors[x];
    }

    template <class T, size_t N, class R1, class R2>
    template <class _R2>
    R2 basic_matrix<T, N, R1, R2>::operator*(const basic_vector<T, N, _R2>& vector) const {
        R2 result;
        for (size_t i = 0; i < N; i++)
            result[i] = vectors[i] * vector;
        return result;
    }

    template <class T, size_t N, class R1, class R2>
    template <class _R1, class _R2>
    R1& basic_matrix<T, N, R1, R2>::operator*=(const basic_matrix<T, N, _R1, _R2>& matrix) {
        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                vectors[x][y] = vectors[x] * matrix.vectors[y];
        return static_cast<R1&>(*this);
    }

    template <class T, size_t N, class R1, class R2>
    template <class _R1, class _R2>
    R1 basic_matrix<T, N, R1, R2>::operator*(const basic_matrix<T, N, _R1, _R2>& matrix) const {
        R1 result;
        for (size_t x = 0; x < N; x++)
            for (size_t y = 0; y < N; y++)
                result[x][y] = vectors[x] * matrix.vectors[y];
        return result;
    }

    template <class T, size_t N, class R1, class R2>
    template <class _R1, class _R2>
    R1 basic_matrix<T, N, R1, R2>::operator+(const basic_matrix<T, N, _R1, _R2>& matrix) const {
        return copy_operator_matrix<std::plus>(matrix);
    }

    template <class T, size_t N, class R1, class R2>
    template <class _R1, class _R2>
    R1 basic_matrix<T, N, R1, R2>::operator-(const basic_matrix<T, N, _R1, _R2>& matrix) const {
        return copy_operator_matrix<std::minus>(matrix);
    }

    template <class T, size_t N, class R1, class R2>
    template <class _R1, class _R2>
    R1& basic_matrix<T, N, R1, R2>::operator+=(const basic_matrix<T, N, _R1, _R2>& matrix) {
        return assign_operator_matrix<std::plus>(matrix);
    }

    template <class T, size_t N, class R1, class R2>
    template <class _R1, class _R2>
    R1& basic_matrix<T, N, R1, R2>::operator-=(const basic_matrix<T, N, _R1, _R2>& matrix) {
        return assign_operator_matrix<std::plus>(matrix);
    }

    template <class T, size_t N, class R1, class R2>
    R1 basic_matrix<T, N, R1, R2>::operator+(T scalar) const {
        return copy_operator_scalar<std::plus>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1 basic_matrix<T, N, R1, R2>::operator-(T scalar) const {
        return copy_operator_scalar<std::minus>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1 basic_matrix<T, N, R1, R2>::operator*(T scalar) const {
        return copy_operator_scalar<std::multiplies>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1 basic_matrix<T, N, R1, R2>::operator/(T scalar) const {
        return copy_operator_scalar<std::divides>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1 basic_matrix<T, N, R1, R2>::operator%(T scalar) const {
        return copy_operator_scalar<std::modulus>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1& basic_matrix<T, N, R1, R2>::operator+=(T scalar) {
        return assign_operator_scalar<std::plus>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1& basic_matrix<T, N, R1, R2>::operator-=(T scalar) {
        return assign_operator_scalar<std::minus>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1& basic_matrix<T, N, R1, R2>::operator*=(T scalar) {
        return assign_operator_scalar<std::multiplies>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1& basic_matrix<T, N, R1, R2>::operator/=(T scalar) {
        return assign_operator_scalar<std::divides>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    R1& basic_matrix<T, N, R1, R2>::operator%=(T scalar) {
        return assign_operator_scalar<std::modulus>(scalar);
    }

    template <class T, size_t N, class R1, class R2>
    std::istream& operator>>(std::istream& in, basic_matrix<T, N, R1, R2>& matrix) {
        for (size_t i = 0; i < N; i++)
            in >> matrix[i];
        return in;
    }

    template <class T, size_t N, class R1, class R2>
    std::ostream& operator<<(std::ostream& out, const basic_matrix<T, N, R1, R2>& matrix) {
        for (size_t i = 0; i < N; i++) {
            out << matrix[i];
            if (i < N - 1)
                out << "\n";
        }
        return out;
    }

    template <class T, size_t N>
    class matrix : public basic_matrix<T, N, matrix<T, N>, vector<T, N>> {
    public:
        matrix() = default;
        explicit matrix(const T (&scalars)[N * N]) noexcept;
        explicit matrix(T scalar) noexcept;
    };

    template <class T, size_t N>
    matrix<T, N>::matrix(const T (&scalars)[N * N]) noexcept
        : basic_matrix<T, N, matrix<T, N>, vector<T, N>>(scalars) {}

    template <class T, size_t N>
    matrix<T, N>::matrix(T scalar) noexcept
        : basic_matrix<T, N, matrix<T, N>, vector<T, N>>(scalar) {}

    template <class T>
    class matrix3d : public basic_matrix<T, 3, matrix3d<T>, vector3d<T>> {
    public:
        matrix3d() = default;
        explicit matrix3d(const T (&scalars)[9]) noexcept;
        explicit matrix3d(T scalar) noexcept;

        static matrix3d<T> rotation_radians(T x_radians, T y_radians, T z_radians);
        static matrix3d<T> rotation_radians(const vector3d<T>& radians);

        static matrix3d<T> rotation_degrees(T x_degrees, T y_degrees, T z_degrees);
        static matrix3d<T> rotation_degrees(const vector3d<T>& degrees);
    };

    template <class T>
    matrix3d<T>::matrix3d(const T (&scalars)[9]) noexcept
        : basic_matrix<T, 3, matrix3d<T>, vector3d<T>>(scalars) {}

    template <class T>
    matrix3d<T>::matrix3d(T scalar) noexcept
        : basic_matrix<T, 3, matrix3d<T>, vector3d<T>>(scalar) {}

    template <class T>
    matrix3d<T> matrix3d<T>::rotation_radians(T x_radians, T y_radians, T z_radians) {
        return {
             cos(y_radians) * cos(z_radians),
             cos(x_radians) * sin(z_radians) + sin(x_radians) * sin(y_radians) * cos(z_radians),
             sin(x_radians) * sin(z_radians) - cos(x_radians) * sin(y_radians) * cos(z_radians),

            -cos(y_radians) * sin(z_radians),
             cos(x_radians) * cos(z_radians) - sin(x_radians) * sin(y_radians) * sin(z_radians),
             sin(x_radians) * cos(z_radians) + cos(x_radians) * sin(y_radians) * sin(z_radians),

             sin(y_radians),
            -sin(x_radians) * cos(y_radians),
             cos(x_radians) * cos(y_radians)
        };
    }

    template <class T>
    matrix3d<T> matrix3d<T>::rotation_radians(const vector3d<T>& radians) {
        return rotation_radians(radians[0], radians[1], radians[2]);
    }

    template <class T>
    matrix3d<T> matrix3d<T>::rotation_degrees(T x_degrees, T y_degrees, T z_degrees) {
        return rotation_radians(to_radians(x_degrees), to_radians(y_degrees), to_radians(z_degrees));
    }

    template <class T>
    matrix3d<T> matrix3d<T>::rotation_degrees(const vector3d<T>& degrees) {
        return rotation_degrees(degrees[0], degrees[1], degrees[2]);
    }
}

// TODO try with inheritance

namespace std {
    template <class T, size_t N, class R>
    struct hash<geometry::basic_vector<T, N, R>> {
        static constexpr hash<T> hash_t;

        template <class _R>
        size_t operator()(const geometry::basic_vector<T, N, _R>& vector) const { // based on: https://stackoverflow.com/a/27216842/8406095
            size_t seed = N;
            for(size_t i = 0; i < N; i++)
                seed ^= hash_t(vector[i]) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
            return seed;
        }
    };

    template <class T, size_t N>
    struct hash<geometry::vector<T, N>> {
        size_t operator()(const geometry::vector<T, N>& vector) const {
            return hash<geometry::basic_vector<T, N, geometry::vector<T, N>>>()(vector);
        }
    };

    template <class T>
    struct hash<geometry::vector3d<T>> {
        size_t operator()(const geometry::vector3d<T>& vector) const {
            return hash<geometry::basic_vector<T, 3, geometry::vector3d<T>>>()(vector);
        }
    };
}

#endif //ZAD5_GEOMETRY_HPP