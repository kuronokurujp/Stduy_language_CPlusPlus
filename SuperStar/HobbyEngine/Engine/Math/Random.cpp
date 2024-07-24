#include "random.h"

namespace Core
{
    namespace Math
    {
        /// <summary>
        /// Initializes this instance.
        /// </summary>
        /// <returns></returns>
        const Bool RandomManager::Init(const Sint32 in_iSeed)
        {
            this->SetSeed(in_iSeed);

            return TRUE;
        }

        /// <summary>
        /// Sets the seed.
        /// </summary>
        /// <param name="in_seed">The in seed.</param>
        /// <returns></returns>
        void RandomManager::SetSeed(const Sint32 in_iSeed)
        {
            this->_generator.seed(in_iSeed);
        }

        /// <summary>
        /// Gets the float range 0 to 1.
        /// </summary>
        /// <returns></returns>
        const Float32 RandomManager::FloatRange0_1()
        {
            return this->Range<Float32>(0.0f, 1.0f);
        }

        /// <summary>
        /// Gets the vector2.
        /// </summary>
        /// <param name="in_rMin">The in r minimum.</param>
        /// <param name="in_rMax">The in r maximum.</param>
        /// <returns></returns>
        /*
        auto Random::GetVector2(const Vector2& in_rMin, const Vector2& in_rMax) -> const Vector2
        {
            Vector2 dist = Vector2(Random::GetFloatRange0_1(), Random::GetFloatRange0_1());
            Vector2 v;
            v.SetMul(in_rMax, dist);
            v += in_rMin;

            return v;
        }
        */
    }  // namespace Math
}  // namespace Core
