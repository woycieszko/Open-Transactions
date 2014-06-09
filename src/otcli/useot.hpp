/* See other files here for the LICENCE that applies here. */
/*
Template for new files, replace word "template" and later delete this line here.
*/

#ifndef INCLUDE_OT_NEWCLI_USEOT
#define INCLUDE_OT_NEWCLI_USEOT

#include "lib_common2.hpp"
//#include "othint.hpp"

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

		bool AccountCheckIfExists(const string & accountName);
		const int64_t AccountGetBalance(const string & accountName);
		const string AccountGetId(const string & accountName);
		const string AccountGetDefault();
		const string AccountGetName(const string & accountID);
		bool AccountRemove(const string & accountName, bool dryrun) ;
		const vector<string> AccountGetAllNames();
		bool AccountDisplayAllNames(bool dryrun);
		const vector<string> AccountGetAllIds();
		bool AccountRefresh(const string & accountName, bool dryrun);
		void AccountRefreshAll(); //TODO remove
		bool AccountRename(const string & oldAccountName, const string & newAccountName, bool dryrun);
		bool AccountSetDefault(const string & accountName, bool dryrun);
		bool AccountSetName(const string & accountID, const string & NewAccountName);
		bool AccountCreate(const string & assetName, const string & newAccountName, bool dryrun);


		bool AssetCheckIfExists(const string & assetName);
		const string AssetGetId(const string & assetName);
		const vector<string> AssetGetAllNames();
		bool AssetDisplayAllNames(bool dryrun);
		bool AssetIssue(const string & serverID, const string & nymID, bool dryrun) ;
		bool AssetNew(const std::string & nymID, bool dryrun);
		const string AssetGetContract(const std::string & assetID);
		const string AssetGetDefault(); // Also known as purse
		void AssetRemove(const string & assetName);
		void AssetSetDefault(const std::string & assetName); // Also known as purse

		const string ContractSign(const std::string & nymID, const std::string & contract);

		const vector<string> MsgGetAll();
		bool MsgDisplayForNym(const string & nymName, bool dryrun);
		bool MsgSend(const string & nymSender, vector<string> nymRecipient, const string & msg, const string & subject, int prio, bool dryrun);
		bool MsgSend(const string & nymSender, const string & nymRecipient, const string & msg);
		bool MsgInCheckIndex(const string & nymName, const int32_t & nIndex);
		bool MsgOutCheckIndex(const string & nymName, const int32_t & nIndex);
		bool MsgInRemoveByIndex(const string & nymName, const int32_t & nIndex, bool dryrun);
		bool MsgOutRemoveByIndex(const string & nymName, const int32_t & nIndex, bool dryrun);

		bool NymCheck(const string & nymName, bool dryrun);
		bool NymCheckIfExists(const string & nymName);
		bool NymCreate(const string & nymName, bool dryrun);
		void NymGetAll();
		const vector<string> NymGetAllIDs();
		const vector<string> NymGetAllNames();
		bool NymDisplayAllNames(bool dryrun);
		const string NymGetDefault();
		const string NymGetId(const string & nymName);
		bool NymDisplayInfo(const string & nymName, bool dryrun);
		const string NymGetName(const string & nymID);
		bool NymRefresh(const string & nymName, bool all, bool dryrun);
		bool NymRegister(const string & nymName, const string & serverName, bool dryrun);
		bool NymRemove(const string & nymName, bool dryrun);
		bool NymSetDefault(const string & nymName, bool dryrun);

		bool ServerAdd(bool dryrun);
		void ServerCheck();
		bool ServerCheckIfExists(const string & serverName);
		const string ServerGetDefault();
		const string ServerGetId(const string & serverName);
		const string ServerGetName(const string & serverID);
		bool ServerRemove(const string & serverName, bool dryrun);
		bool ServerSetDefault(const string & serverName, bool dryrun);
		const vector<string> ServerGetAllNames();
		bool ServerDisplayAllNames(bool dryrun);

		bool TextEncode(const string & plainText, bool dryrun);
		bool TextEncrypt(const string & recipientNymName, const string & plainText, bool dryrun);
		bool TextDecode(const string & encodedText, bool dryrun);
		bool TextDecrypt(const string & recipientNymName, const string & encryptedText, bool dryrun);
	};

} // nUse
} // namespace nOT

#endif

