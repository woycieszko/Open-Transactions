/* See other files here for the LICENCE that applies here. */
/*
Template for new files, replace word "template" and later delete this line here.
*/

#ifndef INCLUDE_OT_NEWCLI_USEOT
#define INCLUDE_OT_NEWCLI_USEOT

#include "lib_common2.hpp"

// Use this to mark methods
#define	EXEC
#define	HINT
#define	VALID

namespace nOT {
namespace nUse {

	INJECT_OT_COMMON_USING_NAMESPACE_COMMON_2; // <=== namespaces

	using ID = string;
	using name = string;

	class cUseOT {

		string mDbgName;

		map<string, ID> mDefaultIDs;
		const string mDataFolder;
		const string mDefaultIDsFile;

		void LoadDefaults();

		public:

		map<ID, name> mNyms; // TODO optimize/share memory? or convert on usage
//		TODO make cache for accounts, assets etc
		bool mNymsMy_loaded;
		bool OTAPI_loaded;
		bool OTAPI_error;

		public:

		cUseOT(const string &mDbgName);
		~cUseOT();

		string DbgName() const noexcept;

		bool Init();
		void CloseApi();

		VALID bool AccountCheckIfExists(const string & accountName);
		const int64_t AccountGetBalance(const string & accountName);
		const string AccountGetId(const string & accountName);
		const string AccountGetDefault();
		const string AccountGetName(const string & accountID);
		EXEC bool AccountRemove(const string & accountName, bool dryrun) ;
		const vector<string> AccountGetAllNames();
		EXEC bool AccountDisplayAllNames(bool dryrun);
		const vector<string> AccountGetAllIds();
		EXEC bool AccountRefresh(const string & accountName, bool all, bool dryrun);
		EXEC bool AccountRename(const string & oldAccountName, const string & newAccountName, bool dryrun);
		EXEC bool AccountSetDefault(const string & accountName, bool dryrun);
		bool AccountSetName(const string & accountID, const string & NewAccountName);
		EXEC bool AccountCreate(const string & assetName, const string & newAccountName, bool dryrun);
		EXEC bool AccountTransfer(const string & accountFrom, const string & accountTo, const int64_t & amount, const string & note, bool dryrun);

		VALID bool AssetCheckIfExists(const string & assetName);
		const string AssetGetId(const string & assetName);
		const vector<string> AssetGetAllNames();
		const string AssetGetName(const string & accountID);
		EXEC bool AssetDisplayAllNames(bool dryrun);
		EXEC bool AssetIssue(const string & serverID, const string & nymID, bool dryrun) ;
		EXEC bool AssetNew(const std::string & nymID, bool dryrun);
		const string AssetGetContract(const std::string & assetID);
		const string AssetGetDefault(); // Also known as purse
		void AssetRemove(const string & assetName);
		void AssetSetDefault(const std::string & assetName); // Also known as purse

		const string ContractSign(const std::string & nymID, const std::string & contract);

		const vector<string> MsgGetAll();
		EXEC bool MsgDisplayForNym(const string & nymName, bool dryrun);
		EXEC bool MsgSend(const string & nymSender, vector<string> nymRecipient, const string & msg, const string & subject, int prio, bool dryrun);
		bool MsgSend(const string & nymSender, const string & nymRecipient, const string & msg);
		VALID bool MsgInCheckIndex(const string & nymName, const int32_t & nIndex);
		VALID bool MsgOutCheckIndex(const string & nymName, const int32_t & nIndex);
		EXEC bool MsgInRemoveByIndex(const string & nymName, const int32_t & nIndex, bool dryrun);
		EXEC bool MsgOutRemoveByIndex(const string & nymName, const int32_t & nIndex, bool dryrun);

		EXEC bool NymCheck(const string & nymName, bool dryrun);
		VALID bool NymCheckIfExists(const string & nymName);
		EXEC bool NymCreate(const string & nymName, bool dryrun);
		void NymGetAll();
		const vector<string> NymGetAllIDs();
		const vector<string> NymGetAllNames();
		EXEC bool NymDisplayAllNames(bool dryrun);
		const string NymGetDefault();
		const string NymGetId(const string & nymName);
		EXEC bool NymDisplayInfo(const string & nymName, bool dryrun);
		const string NymGetName(const string & nymID);
		EXEC bool NymRefresh(const string & nymName, bool all, bool dryrun);
		EXEC bool NymRegister(const string & nymName, const string & serverName, bool dryrun);
		EXEC bool NymRemove(const string & nymName, bool dryrun);
		bool NymSetName(const string & nymID, const string & newNymName);
		bool NymRename(const string & oldNymName, const string & newNymName, bool dryrun);
		EXEC bool NymSetDefault(const string & nymName, bool dryrun);

		EXEC bool ServerAdd(bool dryrun); ///< Add new server contract
		void ServerCheck(); ///< Check server availability (ping server)
		EXEC bool ServerCreate(const string & nymName, bool dryrun); ///< Create new server contract
		VALID bool ServerCheckIfExists(const string & serverName);
		const string ServerGetDefault();
		EXEC const string ServerGetId(const string & serverName);
		const string ServerGetName(const string & serverID);
		EXEC bool ServerRemove(const string & serverName, bool dryrun);
		EXEC bool ServerSetDefault(const string & serverName, bool dryrun);
		const vector<string> ServerGetAllNames();
		EXEC bool ServerDisplayAllNames(bool dryrun);

		EXEC bool TextEncode(const string & plainText, bool dryrun);
		EXEC bool TextEncrypt(const string & recipientNymName, const string & plainText, bool dryrun);
		EXEC bool TextDecode(const string & encodedText, bool dryrun);
		EXEC bool TextDecrypt(const string & recipientNymName, const string & encryptedText, bool dryrun);
	};

} // nUse
} // namespace nOT

#endif

