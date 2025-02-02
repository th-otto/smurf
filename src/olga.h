/* OLGA Rev 1.3 (97-11-30)                       */
/* Thomas_Much@ka2.maus.de                       */
/* http://www.uni-karlsruhe.de/~Thomas.Much/OLGA */

#ifndef OLGA_H
#define OLGA_H


#define OLE_INIT                 0x4950
#define OLE_EXIT                 0x4951
#define OLE_NEW                  0x4952

#define OLGA_INIT                0x1236
#define OLGA_UPDATE              0x1238
#define OLGA_ACK                 0x1239
#define OLGA_RENAME              0x123a
#define OLGA_OPENDOC             0x123b
#define OLGA_CLOSEDOC            0x123c
#define OLGA_LINK                0x123d
#define OLGA_UNLINK              0x123e
#define OLGA_UPDATED             0x123f
#define OLGA_RENAMELINK          0x1240
#define OLGA_LINKRENAMED         0x1241
#define OLGA_GETOBJECTS          0x1242
#define OLGA_OBJECTS             0x1243
#define OLGA_BREAKLINK           0x1244
#define OLGA_LINKBROKEN          0x1245
#define OLGA_START               0x1246
#define OLGA_GETINFO             0x1247
#define OLGA_INFO                0x1248
#define OLGA_IDLE                0x1249
#define OLGA_ACTIVATE            0x124a
#define OLGA_EMBED               0x124b
#define OLGA_EMBEDDED            0x124c
#define OLGA_UNEMBED             0x124d
#define OLGA_GETSETTINGS         0x124e
#define OLGA_SETTINGS            0x124f
#define OLGA_REQUESTNOTIFICATION 0x1250
#define OLGA_RELEASENOTIFICATION 0x1251
#define OLGA_NOTIFY              0x1252
#define OLGA_NOTIFIED            0x1253
#define OLGA_SERVERTERMINATED    0x1254
#define OLGA_CLIENTTERMINATED    0x1255
#define OLGA_INPLACEUPDATE       0x1256
#define OLGA_ID4UPDATE           0x1257
#define OLGA_GETEXTENSION        0x1258
#define OLGA_EXTENSION           0x1259
#define OLGA_GETSERVERPATH       0x125a
#define OLGA_SERVERPATH          0x125b


#define OL_SERVER                0x0001
#define OL_CLIENT                0x0002
#define OL_PEER                  (OL_SERVER | OL_CLIENT)
#define OL_IDLE                  0x0800
#define OL_PIPES                 0x1000
#define OL_START                 0x2000
#define OL_MANAGER               0x4000
#define OL_OEP                   0x0001

#define OLS_TYPE                      1
#define OLS_EXTENSION                 2
#define OLS_NAME                      3

#define OL_SRV_ID4               0x0001


typedef struct
{
	WORD x, y, w, h;
	WORD x1, y1, x2, y2;
} OLGA_GRECT;


typedef struct
{
	long         magic;
	unsigned short version;
	unsigned short skip;
} OLGAInfHeader;


typedef struct
{
	long id;
	long length;
} OLGABlockHeader;


typedef struct
{
	WORD Red;
	WORD Green;
	WORD Blue;
} OLGA_RGB;


typedef struct
{
	WORD Count;
	OLGA_RGB Colors[];
} OLGAColorTable;

#if 0
typedef struct ObjectInfo
{
	char           *Filename;
	AESPB          *ClientGEMPB;
	long            ClientData;
	long            ServerData;
	short           CBLock;
	short           CBCount;
	void            cdecl (*CBDraw)(ObjectInfo *objectinfo, short outScreen, short outHandle, short outDevID, GRECT *Size, GRECT *Clip);
	void            cdecl (*CBUnembed)(ObjectInfo *objectinfo);
	void            cdecl (*CBXDraw)(ObjectInfo *objectinfo, short outScreen, short outHandle, short outDevID, GRECT *Size, GRECT *Clip, long Width_mm1000, long Height_mm1000, long Scale);
	OLGAColorTable *cbColorTable;
	WORD            cbClientID;
	WORD            cbServerID;
} OLGAObjectInfo;
#endif

#endif
