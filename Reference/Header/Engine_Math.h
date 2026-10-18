#ifndef Engine_Math_h__
#define Engine_Math_h__

namespace Engine
{
// ========================================
// Math Helper Functions
// ========================================

/// @brief 값을 지정된 최소/최대 범위 내로 제한
/// @tparam T     비교 연산자(<, >)를 지원하는 타입
/// @param value  제한할 원본 값
/// @param min    최소 한계값
/// @param max    최대 한계값
/// @return       min ~ max 사이로 보정된 값
template <typename T>
constexpr const T& Clamp(const T& value, const T& min, const T& max)
{
    return (value < min) ? min : ((value > max) ? max : value);
}

// ========================================
// Interpolation
// ========================================

/// @brief 부드러운 보간을 위한 감쇠 비율 계산
/// @param speed      감쇠 속도 (값이 클수록 빨리 도착)
/// @param timeDelta  프레임 시간
/// @return           0.0 ~ 1.0 사이의 비율
inline float GetDampingRatio(float speed, float timeDelta)
{
    // 1 - e^(-speed * dt)
    return 1.0f - std::exp(-speed * timeDelta);
}

/// @brief 선형 보간
/// @tparam T     보간 가능한 값 (실수형, 벡터 등 +, -, * 연산자 지원 타입)
/// @param start  보간 시작 값
/// @param end    보간 끝 값
/// @param ratio  비율 (0.f ~ 1.f)
/// @return       start + (end - start) * ratio
template <typename T>
constexpr T Lerp(const T& start, const T& end, float ratio)
{
    return start + (end - start) * ratio;
}

/// @brief 감속 보간
/// @details 목표 지점에 가까워질수록 속도가 줄어들며 부드럽게 정지합니다.
/// @tparam T         보간 가능한 값 (실수형, 벡터 등 +, -, * 연산자 지원 타입)
/// @param start      현재 위치
/// @param end        목표 위치
/// @param speed      감쇠 속도
/// @param timeDelta  프레임 시간
template <typename T>
T Lerp_Damp(const T& start, const T& end, float speed, float timeDelta)
{
    float ratio = GetDampingRatio(speed, timeDelta);
    return Lerp(start, end, ratio);
}

/// @brief 가속 보간
/// @tparam T         보간 가능한 값 (실수형, 벡터 등 +, -, * 연산자 지원 타입)
/// @param initStart  초기 시작 위치
/// @param end        목표 위치
/// @param ratio      진행 비율 (0.f ~ 1.f)
/// @param power      가속 강도. 제곱 연산. 기본값 2.f
template <typename T>
T Lerp_EaseIn(const T& initStart, const T& end, float ratio, float power = 2.0f)
{
    // 비율을 0~1로 안전하게 자름
    ratio = Clamp(ratio, 0.0f, 1.0f);

    // 비율을 제곱하여 곡선을 만듦 (0.5 -> 0.25, 0.9 -> 0.81)
    float easeRatio = std::pow(ratio, power);

    return Lerp(initStart, end, easeRatio);
}
}

#endif // Engine_Math_h__
