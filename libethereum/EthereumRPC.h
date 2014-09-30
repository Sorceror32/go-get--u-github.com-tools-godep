/*
 This file is part of cpp-ethereum.
 
 cpp-ethereum is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 cpp-ethereum is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file EthereumRPC.h
 * @author Alex Leverington <nessence@gmail.com>
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <libdevnet/NetProtocol.h>
#include <libethereum/Interface.h>

namespace dev
{
	
enum EthRequestMsgType : NetMsgType
{
	RequestSubmitTransaction = 0x01,
	RequestCreateContract,
	RequestRLPInject,
	RequestFlushTransactions,
	RequestCallTransaction,
	RequestBalanceAt,
	RequestCountAt,
	RequestStateAt,
	RequestCodeAt,
	RequestStorageAt,
	RequestMessagesWithWatchId,
	RequestMessagesWithFilter,
	InstallWatchWithFilter,
	InstallWatchWithFilterId,
	UninstallWatch,
	PeekWatch,
	CheckWatch,
	Number, // height
	PendingTransactions, // Pending
	Diff,
	GetAddresses, // todo: namespacing
	GasLimitRemaining,
	SetCoinbase, // SetAddress
	GetCoinbase, // GetAddress
	// mining
};
	
//enum P2pRequestMsgType : NetMsgType
//{
//	Peers = 0x01
//	PeerCount
//	ConnectToPeer
//};
	
class EthereumRPCServer;
	
/**
 * @brief Provides network RPC interface as a service endpoint. When EthereumRPC is added to an endpoint it will automatically assign callback methods for the EthereumRPC protocol, which, will interpret and handle RPC requests. As it's a NetService, EthereumRPC can alternatively be used by using registerConnection for each connection which needs to respond to EthereumRPC messages.
 */
class EthereumRPC: public NetService<EthereumRPCServer>
{
public:
	EthereumRPC(eth::Interface* _ethereum): m_ethereum(_ethereum) { }
	
	eth::Interface* ethereum() { return m_ethereum; }
	
protected:
	eth::Interface* m_ethereum;
};
	
	
class EthereumRPC;

/**
 * @brief Provides protocol implementation for handling server-side of Ethereum RPC connections. Each instance handles a single connection and is additionally passed a pointer to EthereumRPC service.
 * All incoming (call) requests receive a response which has the same sequence id of the request.
 */
class EthereumRPCServer: public NetServiceProtocol<EthereumRPC>
{
public:
	static NetMsgServiceType serviceId() { return EthereumService; }
	EthereumRPCServer(NetConnection* _conn, NetServiceFace* _service): NetServiceProtocol(_conn, _service) {};
	void receiveMessage(NetMsg const& _msg);
};

/**
 * @brief Provides protocol implementation and state for handling client-side of Ethereum RPC connections.
 * @todo derive from NetRPCProtocol
 */
class EthereumRPCClient: public NetRPCClientProtocol<EthereumRPCClient>, public eth::Interface
{
public:
	static NetMsgServiceType serviceId() { return EthereumService; }
	EthereumRPCClient(NetConnection* _conn): NetRPCClientProtocol(_conn) {}

public:
	void transact(Secret _secret, u256 _value, Address _dest, bytes const& _data = bytes(), u256 _gas = 10000, u256 _gasPrice = 10 * eth::szabo);
	Address transact(Secret _secret, u256 _endowment, bytes const& _init, u256 _gas = 10000, u256 _gasPrice = 10 * eth::szabo);
	void inject(bytesConstRef _rlp);
	void flushTransactions();
	bytes call(Secret _secret, u256 _value, Address _dest, bytes const& _data = bytes(), u256 _gas = 10000, u256 _gasPrice = 10 * eth::szabo);
	u256 balanceAt(Address _a) const { return balanceAt(_a, m_default); }
	u256 countAt(Address _a) const { return countAt(_a, m_default); }
	u256 stateAt(Address _a, u256 _l) const { return stateAt(_a, _l, m_default); }
	bytes codeAt(Address _a) const { return codeAt(_a, m_default); }
	std::map<u256, u256> storageAt(Address _a) const { return storageAt(_a, m_default); }
	u256 balanceAt(Address _a, int _block) const;
	u256 countAt(Address _a, int _block) const;
	u256 stateAt(Address _a, u256 _l, int _block) const;
	bytes codeAt(Address _a, int _block) const;
	std::map<u256, u256> storageAt(Address _a, int _block) const;
	eth::PastMessages messages(unsigned _watchId) const;
	eth::PastMessages messages(eth::MessageFilter const& _filter) const;
	unsigned installWatch(eth::MessageFilter const& _filter);
	unsigned installWatch(h256 _filterId);
	void uninstallWatch(unsigned _watchId);
	bool peekWatch(unsigned _watchId) const;
	bool checkWatch(unsigned _watchId);
	unsigned number() const;
	
	/// Get a map containing each of the pending transactions.
	/// @TODO: Remove in favour of transactions().
	eth::Transactions pending() const;
	eth::StateDiff diff(unsigned _txi, h256 _block) const;
	eth::StateDiff diff(unsigned _txi, int _block) const;
	Addresses addresses() const { return addresses(m_default); }
	Addresses addresses(int _block) const;
	u256 gasLimitRemaining() const;
	void setAddress(Address _us);
	Address address() const;
	
	void setMiningThreads(unsigned _threads = 0) {};
	unsigned miningThreads() const {};
	void startMining() {};
	void stopMining() {};
	bool isMining() {};
	eth::MineProgress miningProgress() const {};
	
};

	
	
	
}


