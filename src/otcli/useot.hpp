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
	public:

		map<ID, name> mNyms; // TODO optimize/share memory? or convert on usage
//		TODO make cache for accounts, assets etc
		bool mNymsMy_loaded;
		bool OTAPI_loaded;
		bool OTAPI_error;
		enum class subjectType {Account, Asset, Nym, Server};

	private:

		string mDbgName;

		map<string, ID> mDefaultIDs;
		const string mDataFolder;
		const string mDefaultIDsFile;
//		typedef std::function< const ID ( const string & NameOrID ) > tGetID;
		typedef const ID ( cUseOT::*FPTR ) (const string &);

//		map<subjectType, tGetID> subjectGetIDFunc;
		map<subjectType, FPTR> subjectGetIDFunc;

	private:

		void LoadDefaults();

	public:

		cUseOT(const string &mDbgName);
		~cUseOT();

		string DbgName() const noexcept;

		bool Init();
		void CloseApi();

		VALID bool CheckIfExists(subjectType type, const string & subject);

		//================= account =================

		const vector<ID> AccountGetAllIds();
		const int64_t AccountGetBalance(const string & accountName);
		const string AccountGetDefault();
		const ID AccountGetId(const string & account); ///< Gets account ID both from name and ID with prefix
		const string AccountGetName(const ID & accountID);
		bool AccountSetName(const string & accountID, const string & NewAccountName);

		VALID bool AccountCheckIfExists(const string & accountName);

		HINT const vector<string> AccountGetAllNames();

		EXEC bool AccountCreate(const string & assetName, const string & newAccountName, bool dryrun);
		EXEC bool AccountDisplayAllNames(bool dryrun);
		EXEC bool AccountRefresh(const string & accountName, bool all, bool dryrun);
		EXEC bool AccountRemove(const string & account, bool dryrun) ;
		EXEC bool AccountRename(const string & account, const string & newAccountName, bool dryrun);
		EXEC bool AccountSetDefault(const string & account, bool dryrun);
		EXEC bool AccountTransfer(const string & accountFrom, const string & accountTo, const int64_t & amount, const string & note, bool dryrun);

		//================= asset =================

		const ID AssetGetId(const string & asset); ///< Gets asset ID both from name and ID with prefix
		const string AssetGetName(const ID & accountID);
		const string AssetGetContract(const string & asset);
		const string AssetGetDefault(); // Get default asset, also known as purse
		void AssetSetDefault(const std::string & assetName); // Set default asset, also known as purse

		VALID bool AssetCheckIfExists(const string & asset);

		HINT const vector<string> AssetGetAllNames();

		EXEC bool AssetDisplayAllNames(bool dryrun);
		EXEC bool AssetIssue(const string & serverID, const string & nymID, bool dryrun) ;
		EXEC bool AssetNew(const string & nym, bool dryrun);
		EXEC bool AssetRemove(const string & asset, bool dryrun);

		//================= ?contract =================

		const string ContractSign(const std::string & nymID, const std::string & contract);

		//================= msg =================

		const vector<string> MsgGetAll();
		bool MsgSend(const string & nymSender, const string & nymRecipient, const string & msg);

		VALID bool MsgInCheckIndex(const string & nymName, const int32_t & nIndex);
		VALID bool MsgOutCheckIndex(const string & nymName, const int32_t & nIndex);

		EXEC bool MsgDisplayForNym(const string & nymName, bool dryrun);
		EXEC bool MsgSend(const string & nymSender, vector<string> nymRecipient, const string & msg, const string & subject, int prio, bool dryrun);
		EXEC bool MsgInRemoveByIndex(const string & nymName, const int32_t & nIndex, bool dryrun);
		EXEC bool MsgOutRemoveByIndex(const string & nymName, const int32_t & nIndex, bool dryrun);

		//================= nym =================

		void NymGetAll();
		const vector<string> NymGetAllIDs();
		const string NymGetDefault();
		const ID NymGetId(const string & nym); ///< Gets Nym ID both from name and ID with prefix
		const string NymGetName(const string & nymID);
		bool NymSetName(const string & nymID, const string & newNymName);

		VALID bool NymCheckIfExists(const string & nym);

		HINT const vector<string> NymGetAllNames();

		EXEC bool NymCheck(const string & nymName, bool dryrun);
		EXEC bool NymCreate(const string & nymName, bool dryrun);
		EXEC bool NymDisplayAllNames(bool dryrun);
		EXEC bool NymDisplayInfo(const string & nymName, bool dryrun);
		EXEC bool NymRefresh(const string & nymName, bool all, bool dryrun);
		EXEC bool NymRegister(const string & nymName, const string & serverName, bool dryrun);
		EXEC bool NymRemove(const string & nymName, bool dryrun);
		EXEC bool NymRename(const string & oldNymName, const string & newNymName, bool dryrun);
		EXEC bool NymSetDefault(const string & nymName, bool dryrun);

		//================= server =================

		void ServerCheck(); ///< Check server availability (ping server)
		const string ServerGetDefault();
		const ID ServerGetId(const string & server); ///< Gets server ID both from name and ID with prefix
		const string ServerGetName(const string & serverID);

		VALID bool ServerCheckIfExists(const string & server);

		HINT const vector<string> ServerGetAllNames();

		EXEC bool ServerAdd(bool dryrun); ///< Add new server contract
		EXEC bool ServerCreate(const string & nymName, bool dryrun); ///< Create new server contract
		EXEC bool ServerRemove(const string & serverName, bool dryrun);
		EXEC bool ServerSetDefault(const string & serverName, bool dryrun);
		EXEC bool ServerDisplayAllNames(bool dryrun);

		//================= text =================

		EXEC bool TextEncode(const string & plainText, bool dryrun);
		EXEC bool TextEncrypt(const string & recipientNymName, const string & plainText, bool dryrun);
		EXEC bool TextDecode(const string & encodedText, bool dryrun);
		EXEC bool TextDecrypt(const string & recipientNymName, const string & encryptedText, bool dryrun);
	};

} // nUse
} // namespace nOT

#endif

