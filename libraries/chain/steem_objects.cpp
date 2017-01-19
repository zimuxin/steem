
#include <steemit/chain/steem_objects.hpp>
#include <steemit/chain/util/uint256.hpp>

#include <fc/uint128.hpp>

namespace steemit { namespace chain {

asset reward_pool_object::execute_claim( const fc::uint128_t& claim, const fc::time_point_sec& now )
{
   if( claim == 0 )
      return asset( 0, rewards_balance.symbol );

   if( recent_claims_update_time == fc::time_point_sec() )
   {
      asset reward = rewards_balance;
      rewards_balance = asset( 0, rewards_balance.symbol );
      recent_claims += claim;
      recent_claims_update_time = now;
      return reward;
   }

   int64_t dt = (now - recent_claims_update_time).to_seconds();
   FC_ASSERT( dt >= 0 );

   // decay per second, times seconds, equals decay

   uint64_t recent_claims_decay_pct = uint64_t( recent_claims_decay_per_second ) * uint64_t(dt);

   fc::uint128_t recent_claims_decay;
   u256 recent_claims_u256 = util::to256( recent_claims );
   u256 recent_claims_decay_u256 = (recent_claims_u256 * recent_claims_decay_pct) >> 32;
   if( recent_claims_decay_u256 <= recent_claims_u256 )
   {
      // multiply recent_claims by recent_claims_decay_pct, divide by 2**32 and round down

      fc::uint128_t hi = recent_claims.hi;
      hi *= recent_claims_decay_pct;
      fc::uint128_t lo = recent_claims.lo;
      lo *= recent_claims_decay_pct;
      recent_claims_decay = hi+lo.hi;
   }

   if( recent_claims_decay < recent_claims )
      recent_claims -= recent_claims_decay;
   else
      recent_claims = 0;

   // be sure the addition won't overflow
   // can't have claim == 0 as we checked for that above
   FC_ASSERT( recent_claims + claim > recent_claims );
   recent_claims += claim;
   recent_claims_update_time = now;

   u256 reward_amount_u256 = util::to256( claim );
   reward_amount_u256 *= rewards_balance.amount.value;
   reward_amount_u256 /= util::to256( recent_claims );

   // should always hold since it's rewards_balance * F where
   // F = claim / (recent_claims + claim) < 1
   FC_ASSERT( reward_amount_u256 <= u256( rewards_balance.amount.value ) );
   int64_t reward_amount = static_cast< int64_t >( reward_amount_u256 );
   asset reward = asset( reward_amount, rewards_balance.symbol );

   rewards_balance -= reward;

   return reward;
}

} } // steemit::chain
