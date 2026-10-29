#include <cmath>
#include <complex>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>

// Проверка, является ли тип комплексным числом
template <typename T>
struct is_complex {
    static const bool value = false;
};

template <typename T>
struct is_complex<std::complex<T> > {
    static const bool value = true;
};

template <typename T>
class matrix {
private:
    std::size_t rows_;
    std::size_t cols_;
    T* data_;

    static bool is_zero(const T& value) {
        if (is_complex<T>::value) {
            return std::abs(value) < k_epsilon;
        }
        if (std::is_floating_point<T>::value) {
            return std::abs(value) < k_epsilon;
        }
        return value == T(0);
    }

    static bool almost_equal(const T& left, const T& right) {
        if (is_complex<T>::value) {
            return std::abs(left - right) < k_epsilon;
        }
        if (std::is_floating_point<T>::value) {
            return std::abs(left - right) < k_epsilon;
        }
        return left == right;
    }

public:
    static const double k_epsilon;

    matrix() {
        rows_ = 0;
        cols_ = 0;
        data_ = nullptr;
    }

    matrix(std::size_t rows, std::size_t cols, const T& value) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimensions must be positive");
        }
        rows_ = rows;
        cols_ = cols;
        data_ = new T[rows_ * cols_];
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            data_[i] = value;
        }
    }

    matrix(std::size_t rows, std::size_t cols, double low, double high) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimensions must be positive");
        }
        if (low > high) {
            throw std::invalid_argument("Low bound must not exceed high bound");
        }
        rows_ = rows;
        cols_ = cols;
        data_ = new T[rows_ * cols_];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(low, high);
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            data_[i] = static_cast<T>(dist(gen));
        }
    }

    matrix(const matrix& other) {
        rows_ = other.rows_;
        cols_ = other.cols_;
        if (other.data_ != nullptr) {
            data_ = new T[rows_ * cols_];
            for (std::size_t i = 0; i < rows_ * cols_; ++i) {
                data_[i] = other.data_[i];
            }
        } else {
            data_ = nullptr;
        }
    }

    matrix& operator=(const matrix& other) {
        if (this == &other) {
            return *this;
        }
        delete[] data_;
        rows_ = other.rows_;
        cols_ = other.cols_;
        if (other.data_ != nullptr) {
            data_ = new T[rows_ * cols_];
            for (std::size_t i = 0; i < rows_ * cols_; ++i) {
                data_[i] = other.data_[i];
            }
        } else {
            data_ = nullptr;
        }
        return *this;
    }

    ~matrix() {
        delete[] data_;
    }

    std::size_t get_rows() const {
        return rows_;
    }

    std::size_t get_cols() const {
        return cols_;
    }

    T& operator()(std::size_t row, std::size_t col) {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Matrix index out of range");
        }
        return data_[row * cols_ + col];
    }

    const T& operator()(std::size_t row, std::size_t col) const {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Matrix index out of range");
        }
        return data_[row * cols_ + col];
    }

    bool operator==(const matrix& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            return false;
        }
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            if (!almost_equal(data_[i], other.data_[i])) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const matrix& other) const {
        return !(*this == other);
    }

        // Арифметика с присваиванием
    matrix& operator+=(const matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw std::invalid_argument("Matrix dimensions must be equal");
        }
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            data_[i] += other.data_[i];
        }
        return *this;
    }

    matrix& operator-=(const matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw std::invalid_argument("Matrix dimensions must be equal");
        }
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            data_[i] -= other.data_[i];
        }
        return *this;
    }

    matrix& operator*=(const T& scalar) {
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            data_[i] *= scalar;
        }
        return *this;
    }

    matrix& operator/=(const T& scalar) {
        if (is_zero(scalar)) {
            throw std::invalid_argument("Division by zero");
        }
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            data_[i] /= scalar;
        }
        return *this;
    }

    // Бинарная арифметика
    matrix operator+(const matrix& other) const {
        matrix result(*this);
        result += other;
        return result;
    }

    matrix operator-(const matrix& other) const {
        matrix result(*this);
        result -= other;
        return result;
    }

    matrix operator*(const matrix& other) const {
        if (cols_ != other.rows_) {
            throw std::invalid_argument("Matrix dimensions are incompatible");
        }
        matrix result(rows_, other.cols_, T(0));
        for (std::size_t i = 0; i < rows_; ++i) {
            for (std::size_t k = 0; k < cols_; ++k) {
                for (std::size_t j = 0; j < other.cols_; ++j) {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return result;
    }

    matrix operator*(const T& scalar) const {
        matrix result(*this);
        result *= scalar;
        return result;
    }

    matrix operator/(const T& scalar) const {
        matrix result(*this);
        result /= scalar;
        return result;
    }

    T trace() const {
        if (rows_ != cols_) {
            throw std::invalid_argument("Trace is defined only for square matrices");
        }
        T sum = T(0);
        for (std::size_t i = 0; i < rows_; ++i) {
            sum += (*this)(i, i);
        }
        return sum;
    }
};

template <typename T>
const double matrix<T>::k_epsilon = 1e-9;

template <typename T>
matrix<T> operator*(const T& scalar, const matrix<T>& m) {
    return m * scalar;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const matrix<T>& m) {
    for (std::size_t i = 0; i < m.get_rows(); ++i) {
        for (std::size_t j = 0; j < m.get_cols(); ++j) {
            os << m(i, j);
            if (j + 1 < m.get_cols()) {
                os << ' ';
            }
        }
        os << '\n';
    }
    return os;
}

// ---------- Задача №2: обратная матрица 3x3 ----------

template <typename T>
T determinant_3x3(const matrix<T>& m) {
    return m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
         - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
         + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
}

template <typename T>
bool is_zero_value(const T& value) {
    if (is_complex<T>::value) {
        return std::abs(value) < matrix<T>::k_epsilon;
    }
    if (std::is_floating_point<T>::value) {
        return std::abs(value) < matrix<T>::k_epsilon;
    }
    return value == T(0);
}

template <typename T>
matrix<T> inverse_3x3(const matrix<T>& m) {
    if (m.get_rows() != 3 || m.get_cols() != 3) {
        throw std::invalid_argument("Inverse is implemented only for 3x3 matrices");
    }

    T det = determinant_3x3(m);
    if (is_zero_value(det)) {
        throw std::invalid_argument("Matrix is singular and has no inverse");
    }

    matrix<T> cofactors(3, 3, T(0));

    cofactors(0, 0) = m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1);
    cofactors(0, 1) = -(m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0));
    cofactors(0, 2) = m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0);

    cofactors(1, 0) = -(m(0, 1) * m(2, 2) - m(0, 2) * m(2, 1));
    cofactors(1, 1) = m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0);
    cofactors(1, 2) = -(m(0, 0) * m(2, 1) - m(0, 1) * m(2, 0));

    cofactors(2, 0) = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);
    cofactors(2, 1) = -(m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0));
    cofactors(2, 2) = m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);

    matrix<T> adjugate(3, 3, T(0));
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            adjugate(j, i) = cofactors(i, j);
        }
    }

    return adjugate / det;
}

int main() {
    std::cout << "=== Matrix demonstration ===\n";

    matrix<double> empty;
    std::cout << "Empty matrix: rows = " << empty.get_rows() 
              << ", cols = " << empty.get_cols() << "\n\n";

    matrix<double> filled(3, 3, 1.5);
    std::cout << "Filled:\n" << filled;

    matrix<double> random_matrix(3, 3, -5.0, 5.0);
    std::cout << "Random (rows=" << random_matrix.get_rows() 
              << ", cols=" << random_matrix.get_cols() << "):\n" << random_matrix;

    matrix<double> copy_matrix(random_matrix);
    std::cout << "Copy of random:\n" << copy_matrix;

    matrix<double> assigned_matrix;
    assigned_matrix = random_matrix;
    std::cout << "Assigned copy:\n" << assigned_matrix;

    matrix<double> a(3, 3, 1.0);
    matrix<double> b(3, 3, 2.0);
    std::cout << "A + B:\n" << (a + b);
    std::cout << "A - B:\n" << (a - b);
    std::cout << "A * B:\n" << (a * b);
    std::cout << "A * 2.0:\n" << (a * 2.0);
    std::cout << "2.0 * A:\n" << (2.0 * a);
    std::cout << "A / 2.0:\n" << (a / 2.0);

    matrix<double> c = a;
    c += b;
    std::cout << "A += B:\n" << c;

    c = a;
    c -= b;
    std::cout << "A -= B:\n" << c;

    c = b;
    c *= 3.0;
    std::cout << "B *= 3.0:\n" << c;

    c = b;
    c /= 2.0;
    std::cout << "B /= 2.0:\n" << c;

    std::cout << "Trace(A): " << a.trace() << "\n";

    std::cout << "A == B? " << (a == b ? "yes" : "no") << "\n";
    std::cout << "A != B? " << (a != b ? "yes" : "no") << "\n\n";

    // Задача: обратная матрица
    std::cout << "=== Inverse 3x3 ===\n";
    std::cout << "Original A:\n" << random_matrix;
    try {
        matrix<double> inv = inverse_3x3(random_matrix);
        std::cout << "A^-1:\n" << inv;
        std::cout << "A * A^-1:\n" << (random_matrix * inv);
    } catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << "\n";
    }

    matrix<double> singular(3, 3, 0.0);
    singular(0, 0) = 1; singular(0, 1) = 2; singular(0, 2) = 3;
    singular(1, 0) = 4; singular(1, 1) = 5; singular(1, 2) = 6;
    singular(2, 0) = 7; singular(2, 1) = 8; singular(2, 2) = 9;
    try {
        matrix<double> inv = inverse_3x3(singular);
    } catch (const std::exception& ex) {
        std::cout << "Singular matrix: " << ex.what() << "\n";
    }

    return 0;
}