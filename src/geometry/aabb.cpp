#include "aabb.h"

#include <algorithm>
#include <cmath>
#include <ostream>

#ifdef __AVX__
#include <immintrin.h>
#endif

#include "math/point.h"
#include "math/ray.h"

void AABB::set_axes(math::Interval x, math::Interval y, math::Interval z) {
    min_ = {x.min, y.min, z.min, -math::infinity};
    max_ = {x.max, y.max, z.max, math::infinity};
}

AABB::AABB() {
    set_axes(math::Interval{}, math::Interval{}, math::Interval{});
}

AABB::AABB(math::Interval x, math::Interval y, math::Interval z) {
    set_axes(x.ensure_size(math::bbox_eps), y.ensure_size(math::bbox_eps), z.ensure_size(math::bbox_eps));
}

AABB::AABB(std::initializer_list<Point> points) {
    if (points.size() == 0) {
        set_axes(math::Interval{}, math::Interval{}, math::Interval{});
        return;
    }

    math::Interval x_interval{};  // Empty interval with min = +inf and max = -inf
    math::Interval y_interval{};
    math::Interval z_interval{};

    for (const Point& p : points) {
        x_interval.max = std::max(x_interval.max, p.x());
        x_interval.min = std::min(x_interval.min, p.x());
        y_interval.max = std::max(y_interval.max, p.y());
        y_interval.min = std::min(y_interval.min, p.y());
        z_interval.max = std::max(z_interval.max, p.z());
        z_interval.min = std::min(z_interval.min, p.z());
    }

    set_axes(
        math::Interval{x_interval.min, x_interval.max}.ensure_size(math::bbox_eps),
        math::Interval{y_interval.min, y_interval.max}.ensure_size(math::bbox_eps),
        math::Interval{z_interval.min, z_interval.max}.ensure_size(math::bbox_eps)
    );
}

math::Interval AABB::x() const { return math::Interval{min_[0], max_[0]}; }
math::Interval AABB::y() const { return math::Interval{min_[1], max_[1]}; }
math::Interval AABB::z() const { return math::Interval{min_[2], max_[2]}; }

math::Interval AABB::operator[](std::size_t i) const { return math::Interval{min_[i], max_[i]}; }

AABB AABB::merge(AABB a, AABB b) {
    return AABB{
        math::Interval::merge(a.x(), b.x()),
        math::Interval::merge(a.y(), b.y()),
        math::Interval::merge(a.z(), b.z())
    };
}

std::size_t AABB::longest_axis() {
    std::size_t longest_axis = 0;
    double max_size = 0;
    for (std::size_t axis = 0; axis < 3; ++axis) {
        if ((*this)[axis].size() > max_size) {
            longest_axis = axis;
            max_size = (*this)[axis].size();
        }
    }
    return longest_axis;
}

#ifdef __AVX__
namespace {
    double horizontal_max(__m256d v) {
        __m128d m = _mm_max_pd(_mm256_castpd256_pd128(v), _mm256_extractf128_pd(v, 1));
        return _mm_cvtsd_f64(_mm_max_sd(m, _mm_unpackhi_pd(m, m)));
    }

    double horizontal_min(__m256d v) {
        __m128d m = _mm_min_pd(_mm256_castpd256_pd128(v), _mm256_extractf128_pd(v, 1));
        return _mm_cvtsd_f64(_mm_min_sd(m, _mm_unpackhi_pd(m, m)));
    }
}

bool AABB::hit(const Ray& ray, math::Interval t_range) const {
    const __m256d orig = _mm256_loadu_pd(ray.origin_4().data());
    const __m256d inv_dir = _mm256_loadu_pd(ray.inv_direction_4().data());

    const __m256d t_0 = _mm256_mul_pd(_mm256_sub_pd(_mm256_loadu_pd(min_.data()), orig), inv_dir);
    const __m256d t_1 = _mm256_mul_pd(_mm256_sub_pd(_mm256_loadu_pd(max_.data()), orig), inv_dir);

    // Clamping with t_range as the second operand also replaces NaN lanes (0 * inf), like std::fmin/fmax did
    const __m256d t_near = _mm256_max_pd(_mm256_min_pd(t_0, t_1), _mm256_set1_pd(t_range.min));
    const __m256d t_far = _mm256_min_pd(_mm256_max_pd(t_0, t_1), _mm256_set1_pd(t_range.max));

    return horizontal_min(t_far) > horizontal_max(t_near);
}
#else
bool AABB::hit(const Ray& ray, math::Interval t_range) const {
    const auto& orig = ray.origin_4();
    const auto& inv_dir = ray.inv_direction_4();

    for (int axis = 0; axis < 3; ++axis) {
        double t_0 = (min_[axis] - orig[axis]) * inv_dir[axis];
        double t_1 = (max_[axis] - orig[axis]) * inv_dir[axis];

        t_range.min = std::fmax(std::fmin(t_0, t_1), t_range.min);
        t_range.max = std::fmin(std::fmax(t_0, t_1), t_range.max);

        if (t_range.max <= t_range.min) {
            return false;
        }
    }
    return true;
}
#endif


std::ostream& operator<<(std::ostream& os, const AABB& bbox) {
    return os << "AABB[x" << bbox.x() << ", y" << bbox.y() << ", z" << bbox.z() << "]";
}
