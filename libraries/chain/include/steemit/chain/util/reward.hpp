#pragma once

#include <steemit/chain/util/asset.hpp>

#include <steemit/protocol/asset.hpp>
#include <steemit/protocol/config.hpp>
#include <steemit/protocol/types.hpp>

#include <fc/reflect/reflect.hpp>

#include <fc/uint128.hpp>

namespace steemit { namespace chain { namespace util {

using steemit::protocol::asset;
using steemit::protocol::price;
using steemit::protocol::share_type;

using fc::uint128_t;

struct comment_block_reward
{
   // How much comes from the pool
   asset      available_block_reward;  // total gotten from the pool in initial single tx
   uint128_t  total_block_claims;      // total rshares2 getting reward from this pool
   share_type total_block_reward;      // total paid out, may be smaller than available_block_reward due to rounding
};

struct comment_reward_context
{
   // local values
   share_type rshares;
   uint16_t   reward_weight = 0;
   asset      max_sbd;

   // global values
   // used pre-#774
   uint128_t  total_reward_shares2;
   asset      total_reward_fund_steem;
   price      current_steem_price;

   // used in HF17 comment equalization
   // used post-#774
   fc::array< comment_block_reward, STEEMIT_NUM_REWARD_POOLS > block_reward_for_pool;

   // used post-#774
   int64_t pool_id = 0;
   share_type reward_from_pool;
};

struct comment_reward_result
{
   share_type rshares;
};

uint64_t get_rshare_reward_pre_hf17( const comment_reward_context& ctx );
uint64_t get_rshare_reward( const comment_reward_context& ctx );

inline uint128_t get_content_constant_s()
{
   return uint128_t( uint64_t(2000000000000ll) ); // looking good for posters
}

uint128_t calculate_vshares( const uint128_t& rshares );

inline bool is_comment_payout_dust( const price& p, uint64_t steem_payout )
{
   return to_sbd( p, asset( steem_payout, STEEM_SYMBOL ) ) < STEEMIT_MIN_PAYOUT_SBD;
}

} } }

FC_REFLECT( steemit::chain::util::comment_reward_context,
   (rshares)
   (reward_weight)
   (max_sbd)
   (total_reward_shares2)
   (total_reward_fund_steem)
   (current_steem_price)
   )
