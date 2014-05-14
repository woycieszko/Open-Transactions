/* See other files here for the LICENCE that applies here. */
/*
Template for new files, replace word "template" and later delete this line here.
*/

#ifndef INCLUDE_OT_NEWCLI_USEOT
#define INCLUDE_OT_NEWCLI_USEOT

#include "lib_common2.hpp"
#include "othint.hpp"

namespace nOT {
namespace nUse {

	INJECT_OT_COMMON_USING_NAMESPACE_COMMON_2; // <=== namespaces

	class cUseOT {

		string mServerID;
		string mUserID;
		string mAccountID;
		string mPurseID;

		bool Init();

		public:

		// vector<nOT::nNewcli::cNyminfo> mNymsMy; // FIXME Not used class cNyminfo (look: othint.cpp)
		vector<string> mNymsMy_str; // TODO optimize/share memory? or convert on usage

		bool mNymsMy_loaded;
		bool OTAPI_loaded;
		bool OTAPI_error;

		cUseOT();
		~cUseOT();

		bool accountCheckIfExists(const string & accountName);
		const string accountGetId(const string & accountName);
		string accountDelete(const string & accountName);
		const vector<string> accountGetIds();
		const string accountRename(const string & oldAccountName, const string & newAccountName);
		const string accountSetName(const string & accountID, const string & NewAccountName);
		void accountCreate(const string & assetName, const string & newAccountName);
		const vector<string> accountsGet();

		bool assetCheckIfExists(const string & assetName);
		const string assetGetId(const string & assetName);
		const vector<string> assetsGetNames();

		const vector<string> msgGetAll();
		const vector<string> msgGetForNym(const string & nymName);
		void msgSend(const string & nymSender, const string & nymRecipient, const string & msg);
		void msgRemoveByIndex(const string & nymName, const int32_t & nIndex);

		void nymCheck(const string & hisNymID);
		bool nymCheckByName(const string & nymName);
		void nymCreate(const string & nymName);
		const string nymGetDefault();
		const string nymGetId(const string & nymName);
		const string nymGetInfo(const string & nymName);
		void nymRefresh();
		void nymRegister(const string & nymName);
		void nymRegister(const string & nymName, const string & serverName);
		void nymSetDefault(const string & nymName);
		const vector<string> nymsGetMy();

		const string serverGetDefault();
		const string serverGetId(const string & serverName);
		void serverSetDefault(const string & serverName);
		const vector<string> serversGet();

		const string textEncode(const string & plainText);
		const string textEncrypt(const string & recipientNymName, const string & plainText);
		const string textDecode(const string & encodedText);
		const string textDecrypt(const string & recipientNymName, const string & encryptedText);
	};

	extern cUseOT useOT;

} // nUse
} // namespace nOT

#endif

