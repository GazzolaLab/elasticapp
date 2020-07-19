//*************************************************************************************************
/*!\brief Selection of the coarse collision detection algorithm.
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


//*************************************************************************************************
/*!\brief Selection of the fine collision detection algorithm.
 * \ingroup config
 *
 * This macro selects the fine collision detection algorithm. The purpose of this algorithm is
 * the generation of contact points between colliding rigid bodies. The following algorithms
 * are available for the fine collision detection:
 *   - elastica::collision::detection::fine::MaxContacts
 */
#define ELASTICA_FINE_COLLISION_DETECTOR  elastica::collision::detection::fine::MaxContacts
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Selection of the batch generation algorithm.
 * \ingroup config
 *
 * This macro selects the batch generation algorithm. The purpose of this algorithm is the
 * setup of distinct groups/batches of contacts that can be treated in parallel and independent
 * of each other. The following algorithms are available:
 *   - elastica::collision::batches::SingleBatch
 *   - elastica::collision::batches::UnionFind
 */
#define ELASTICA_BATCH_GENERATOR  elastica::collision::batches::SingleBatch
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Selection of the collision response algorithm.
 * \ingroup config
 *
 * This macro selects the collision response algorithm. The collision response is responsible
 * to calculate contact forces/impulses to handle collisions between rigid bodies. The following
 * solvers are available:
 *   - elastica::collision::response::DEMSolver
 */
#define ELASTICA_CONSTRAINT_SOLVER  elastica::collision::response::DEMSolver
//*************************************************************************************************
