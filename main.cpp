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
};

template <typename T>
const double matrix<T>::k_epsilon = 1e-9;

int main() {
    return 0;
}