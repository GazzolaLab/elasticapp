#pragma once

//*************************************************************************************************
/*!\brief Selection of the environment modules
 * \ingroup config
 *
 * This macro selects the coarse collision detection algorithm. The purpose of this algorithm
 * is used to efficiently sort out all pairs of rigid bodies that are potentially colliding
 * and sending them to the fine collision detection. The following algorithms can be chosen:
 *   - elastica::collision::detection::coarse::ExhaustiveSearch
 *   - elastica::collision::detection::coarse::HashGrids
 *   - elastica::collision::detection::coarse::SweepAndPrune
 */
#define ELASTICA_COARSE_COLLISION_DETECTOR  elastica::collision::detection::coarse::ExhaustiveSearch
//*************************************************************************************************
