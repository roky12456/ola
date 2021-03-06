/**
* Copyright (c) 2018 Zilliqa 
* This source code is being disclosed to you solely for the purpose of your participation in 
* testing Zilliqa. You may view, compile and run the code for that purpose and pursuant to 
* the protocols and algorithms that are programmed into, and intended by, the code. You may 
* not do anything else with the code without express permission from Zilliqa Research Pte. Ltd., 
* including modifying or publishing the code (or any part of it), and developing or forming 
* another public or private blockchain network. This source code is provided ‘as is’ and no 
* warranties are given as to title or non-infringement, merchantability or fitness for purpose 
* and, to the extent permitted by law, all liability for your use of the code is disclaimed. 
* Some programs in this code are governed by the GNU General Public License v3.0 (available at 
* https://www.gnu.org/licenses/gpl-3.0.en.html) (‘GPLv3’). The programs that are governed by 
* GPLv3.0 are those programs that are located in the folders src/depends and tests/depends 
* and which include a reference to GPLv3 in their program files.
**/

#ifndef __DSBLOCKMESSAGE_H__
#define __DSBLOCKMESSAGE_H__

#include "libCrypto/Schnorr.h"
#include "libNetwork/Peer.h"
#include "libUtils/SanityChecks.h"
#include <deque>
#include <map>
#include <utility>
#include <vector>

class ShardingStructure
{
    // Sharding structure message format:
    // [4-byte num of committees]
    // [4-byte committee size]
    //   [33-byte public key] [16-byte ip] [4-byte port]
    //   [33-byte public key] [16-byte ip] [4-byte port]
    //   ...
    // [4-byte committee size]
    //   [33-byte public key] [16-byte ip] [4-byte port]
    //   [33-byte public key] [16-byte ip] [4-byte port]
    //   ...

public:
    static unsigned int
    Serialize(const std::vector<std::map<PubKey, Peer>>& shards,
              std::vector<unsigned char>& output, unsigned int cur_offset)
    {
        LOG_MARKER();

        uint32_t numOfComms = shards.size();

        // 4-byte num of committees
        Serializable::SetNumber<uint32_t>(output, cur_offset, numOfComms,
                                          sizeof(uint32_t));
        cur_offset += sizeof(uint32_t);

        LOG_GENERAL(INFO, "Number of committees = " << numOfComms);

        for (unsigned int i = 0; i < numOfComms; i++)
        {
            const std::map<PubKey, Peer>& shard = shards.at(i);

            // 4-byte committee size
            Serializable::SetNumber<uint32_t>(output, cur_offset, shard.size(),
                                              sizeof(uint32_t));
            cur_offset += sizeof(uint32_t);

            LOG_GENERAL(INFO,
                        "Committee size = " << shard.size() << "\n"
                                            << "Members:");

            for (auto& kv : shard)
            {
                // 33-byte public key
                kv.first.Serialize(output, cur_offset);
                cur_offset += PUB_KEY_SIZE;

                // 16-byte ip + 4-byte port
                kv.second.Serialize(output, cur_offset);
                cur_offset += IP_SIZE + PORT_SIZE;

                LOG_GENERAL(
                    INFO,
                    " PubKey = "
                        << DataConversion::SerializableToHexStr(kv.first)
                        << " at " << kv.second.GetPrintableIPAddress()
                        << " Port: " << kv.second.m_listenPortHost);
            }
        }

        return cur_offset;
    }
};

#endif // __DSBLOCKMESSAGE_H__