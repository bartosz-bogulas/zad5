#ifndef __ZAD5___VECTOR_HPP
#define __ZAD5___VECTOR_HPP

#include <iostream>

template <class T, size_t N>
class Vector {
public:
    Vector() = default;

    T operator[](size_t i) const;
    T& operator[](size_t i);

    Vector<T, N> operator+(const Vector<T, N>& vector) const;
    Vector<T, N> operator-(const Vector<T, N>& vector) const;

    Vector<T, N> operator+(T scalar) const;
    Vector<T, N> operator-(T scalar) const;
    Vector<T, N> operator*(T scalar) const;
    Vector<T, N> operator/(T scalar) const;

    template <typename _T, size_t _N>
    friend std::istream& operator>>(std::istream& in, Vector<_T, _N>& vector);
    template <typename _T, size_t _N>
    friend std::ostream& operator<<(std::ostream& out, const Vector<_T, _N>& vector);

private:
    T scalars[N];
};

template <class T, size_t N>
T Vector<T, N>::operator[](const size_t i) const {
    return scalars[i];
}

template <class T, size_t N>
T& Vector<T, N>::operator[](const size_t i) {
    return scalars[i];
}

template <class T, size_t N>
Vector<T, N> Vector<T, N>::operator+(const Vector<T, N> &vector) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] + vector.scalars[i];
    return result;
}

template <class T, size_t N>
Vector<T, N> Vector<T, N>::operator-(const Vector<T, N> &vector) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] - vector.scalars[i];
    return result;
}

template <class T, size_t N>
Vector<T, N> Vector<T, N>::operator+(T scalar) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] + scalar;
    return result;
}

template <class T, size_t N>
Vector<T, N> Vector<T, N>::operator-(T scalar) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] - scalar;
    return result;
}

template <class T, size_t N>
Vector<T, N> Vector<T, N>::operator*(T scalar) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] * scalar;
    return result;
}

template <class T, size_t N>
Vector<T, N> Vector<T, N>::operator/(T scalar) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; i++)
        result.scalars[i] = scalars[i] / scalar;
    return result;
}

template <class T, size_t N>
std::istream& operator>>(std::istream& in, Vector<T, N>& vector) {
    for (size_t i = 0; i < N; i++)
        in >> vector.scalars[i];
    return in;
}

template <class T, size_t N>
std::ostream& operator<<(std::ostream& out, const Vector<T, N>& vector) {
    for (size_t i = 0; i < N; i++) {
        out << vector.scalars[i];
        if (i < N - 1)
            out << " ";
    }
    return out;
}

using Vector3D = Vector<double, 3>;

#endif //__ZAD5___VECTOR_HPP
