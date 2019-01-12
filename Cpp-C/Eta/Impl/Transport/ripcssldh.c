/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include <openssl/dh.h>

#ifndef WIN32
#include "rtr/ripcsslutils.h"
#include "rtr/ripcssljit.h"

/* ----BEGIN GENERATED SECTION-------- */
/*
** DH Parameters: (512 bit)
**     prime:
**         00:f1:a1:cb:66:28:85:9c:29:05:1e:79:7e:4b:2e:
**         22:3e:4b:d4:38:f9:f3:dd:85:3f:5f:cc:bd:90:8e:
**         b9:6c:eb:b0:87:62:31:92:d5:84:6e:21:86:6d:f0:
**         7b:e2:75:47:d2:10:6c:9a:0c:a0:33:6a:be:04:ba:
**         9d:6d:7a:92:c3
**     generator: 2 (0x2)
** DH Parameters: (1024 bit)
**     prime:
**         00:97:54:f1:9f:df:02:96:be:e5:01:9e:e8:d4:06:
**         7d:6f:15:ad:b6:35:7b:da:90:b0:50:7e:1a:71:ff:
**         40:d4:f7:59:fe:45:ab:01:7a:b2:c1:79:f6:56:7a:
**         dc:9a:a1:f5:0e:1e:46:e8:15:6b:d1:c2:c4:a8:27:
**         7f:a9:fa:dc:35:a9:5c:18:4b:dd:e3:39:1b:93:3c:
**         a0:25:a3:93:19:29:18:6f:51:aa:22:a1:32:d8:35:
**         68:64:b6:18:47:f3:01:a2:5f:2f:44:2e:48:6b:ea:
**         7e:27:95:47:05:9d:8b:24:fb:2f:7d:cf:a4:e0:5f:
**         1f:7f:08:f8:c8:7c:a0:6f:cb
**      generator: 2 (0x2)
** DH Parameters: (2048 bit)
**     prime:
**          00:c3:53:f8:2e:62:af:aa:54:ad:83:6b:e1:80:11:
**          82:b1:f2:93:0d:d1:19:bc:3f:aa:ef:28:e8:1d:38:
**          a3:3b:5f:6a:98:e8:c2:cd:55:9b:da:d4:38:bf:3e:
**          57:c4:8e:2d:a7:b5:b5:2d:21:0b:29:cf:da:31:0b:
**          c9:fd:1e:b4:cb:31:b0:11:2d:f0:16:e3:42:30:41:
**          9e:7d:54:ed:15:53:b4:fb:a9:4e:41:b1:c6:3c:af:
**          05:ef:bf:db:0c:e8:64:d6:0c:6c:26:e8:6c:ae:b1:
**          2a:85:6b:17:5c:96:b0:a4:5b:1c:57:ad:fc:f0:c1:
**          8d:60:fe:64:cb:20:ec:77:e7:bf:6e:2a:6b:50:9f:
**          e1:86:be:19:9c:83:7b:3b:84:56:08:45:12:25:b4:
**          a9:8f:b2:32:2e:c5:58:62:97:e2:94:a8:62:3b:bf:
**          3f:75:dc:dd:cf:12:34:75:e7:51:46:ab:bf:f5:20:
**          eb:22:ce:8f:71:fe:f3:52:35:9e:8b:27:c7:c4:17:
**          c9:0d:8c:27:38:5b:26:95:a7:b2:e8:42:a4:48:94:
**          8c:b2:2d:8d:bb:ec:9f:fc:48:6e:66:01:b8:d8:c8:
**          3c:5a:fd:79:c3:08:a4:58:b5:fc:6f:7e:f9:fd:61:
**          b0:49:4b:28:2a:c1:9a:81:84:2d:26:f4:bc:43:c7:
**          6b:0b
**      generator: 2 (0x2)
*/

static DH *get_dh512(ripcCryptoApiFuncs* cryptoFuncs)
{
    static unsigned char dhp_512[] = {
        0xF1, 0xA1, 0xCB, 0x66, 0x28, 0x85, 0x9C, 0x29, 0x05, 0x1E,
        0x79, 0x7E, 0x4B, 0x2E, 0x22, 0x3E, 0x4B, 0xD4, 0x38, 0xF9,
        0xF3, 0xDD, 0x85, 0x3F, 0x5F, 0xCC, 0xBD, 0x90, 0x8E, 0xB9,
        0x6C, 0xEB, 0xB0, 0x87, 0x62, 0x31, 0x92, 0xD5, 0x84, 0x6E,
        0x21, 0x86, 0x6D, 0xF0, 0x7B, 0xE2, 0x75, 0x47, 0xD2, 0x10,
        0x6C, 0x9A, 0x0C, 0xA0, 0x33, 0x6A, 0xBE, 0x04, 0xBA, 0x9D,
        0x6D, 0x7A, 0x92, 0xC3
    };
    static unsigned char dhg_512[] = {
            0x02
    };
    DH *dh = (*(cryptoFuncs->dh_new)) ();
    BIGNUM *p, *g;

    if (dh == NULL)
        return NULL;
    p = (*(cryptoFuncs->bin2bn)) (dhp_512, sizeof (dhp_512), NULL);
    g = (*(cryptoFuncs->bin2bn)) (dhg_512, sizeof (dhg_512), NULL);
    if (p == NULL || g == NULL || !(*(cryptoFuncs->dh_set_pqg)) (dh, p, NULL, g))
    {
        (*(cryptoFuncs->dh_free)) (dh);
        (*(cryptoFuncs->bn_free)) (p);
        (*(cryptoFuncs->bn_free)) (g);
        return NULL;
    }
    return dh;
}

static DH *get_dh1024(ripcCryptoApiFuncs* cryptoFuncs)
{
    static unsigned char dhp_1024[] = {
        0x97, 0x54, 0xF1, 0x9F, 0xDF, 0x02, 0x96, 0xBE, 0xE5, 0x01,
        0x9E, 0xE8, 0xD4, 0x06, 0x7D, 0x6F, 0x15, 0xAD, 0xB6, 0x35,
        0x7B, 0xDA, 0x90, 0xB0, 0x50, 0x7E, 0x1A, 0x71, 0xFF, 0x40,
        0xD4, 0xF7, 0x59, 0xFE, 0x45, 0xAB, 0x01, 0x7A, 0xB2, 0xC1,
        0x79, 0xF6, 0x56, 0x7A, 0xDC, 0x9A, 0xA1, 0xF5, 0x0E, 0x1E,
        0x46, 0xE8, 0x15, 0x6B, 0xD1, 0xC2, 0xC4, 0xA8, 0x27, 0x7F,
        0xA9, 0xFA, 0xDC, 0x35, 0xA9, 0x5C, 0x18, 0x4B, 0xDD, 0xE3,
        0x39, 0x1B, 0x93, 0x3C, 0xA0, 0x25, 0xA3, 0x93, 0x19, 0x29,
        0x18, 0x6F, 0x51, 0xAA, 0x22, 0xA1, 0x32, 0xD8, 0x35, 0x68,
        0x64, 0xB6, 0x18, 0x47, 0xF3, 0x01, 0xA2, 0x5F, 0x2F, 0x44,
        0x2E, 0x48, 0x6B, 0xEA, 0x7E, 0x27, 0x95, 0x47, 0x05, 0x9D,
        0x8B, 0x24, 0xFB, 0x2F, 0x7D, 0xCF, 0xA4, 0xE0, 0x5F, 0x1F,
        0x7F, 0x08, 0xF8, 0xC8, 0x7C, 0xA0, 0x6F, 0xCB
    };
    static unsigned char dhg_1024[] = {
            0x02
    };
    DH *dh = (*(cryptoFuncs->dh_new)) ();
    BIGNUM *p, *g;

    if (dh == NULL)
        return NULL;
    p = (*(cryptoFuncs->bin2bn)) (dhp_1024, sizeof (dhp_1024), NULL);
    g = (*(cryptoFuncs->bin2bn)) (dhg_1024, sizeof (dhg_1024), NULL);
    if (p == NULL || g == NULL || !(*(cryptoFuncs->dh_set_pqg)) (dh, p, NULL, g))
    {
        (*(cryptoFuncs->dh_free)) (dh);
        (*(cryptoFuncs->bn_free)) (p);
        (*(cryptoFuncs->bn_free)) (g);
        return NULL;
    }
    return dh;
}

static DH *get_dh2048(ripcCryptoApiFuncs* cryptoFuncs)
{
    static unsigned char dhp_2048[] = {
        0xC3, 0x53, 0xF8, 0x2E, 0x62, 0xAF, 0xAA, 0x54, 0xAD, 0x83,
        0x6B, 0xE1, 0x80, 0x11, 0x82, 0xB1, 0xF2, 0x93, 0x0D, 0xD1,
        0x19, 0xBC, 0x3F, 0xAA, 0xEF, 0x28, 0xE8, 0x1D, 0x38, 0xA3,
        0x3B, 0x5F, 0x6A, 0x98, 0xE8, 0xC2, 0xCD, 0x55, 0x9B, 0xDA,
        0xD4, 0x38, 0xBF, 0x3E, 0x57, 0xC4, 0x8E, 0x2D, 0xA7, 0xB5,
        0xB5, 0x2D, 0x21, 0x0B, 0x29, 0xCF, 0xDA, 0x31, 0x0B, 0xC9,
        0xFD, 0x1E, 0xB4, 0xCB, 0x31, 0xB0, 0x11, 0x2D, 0xF0, 0x16,
        0xE3, 0x42, 0x30, 0x41, 0x9E, 0x7D, 0x54, 0xED, 0x15, 0x53,
        0xB4, 0xFB, 0xA9, 0x4E, 0x41, 0xB1, 0xC6, 0x3C, 0xAF, 0x05,
        0xEF, 0xBF, 0xDB, 0x0C, 0xE8, 0x64, 0xD6, 0x0C, 0x6C, 0x26,
        0xE8, 0x6C, 0xAE, 0xB1, 0x2A, 0x85, 0x6B, 0x17, 0x5C, 0x96,
        0xB0, 0xA4, 0x5B, 0x1C, 0x57, 0xAD, 0xFC, 0xF0, 0xC1, 0x8D,
        0x60, 0xFE, 0x64, 0xCB, 0x20, 0xEC, 0x77, 0xE7, 0xBF, 0x6E,
        0x2A, 0x6B, 0x50, 0x9F, 0xE1, 0x86, 0xBE, 0x19, 0x9C, 0x83,
        0x7B, 0x3B, 0x84, 0x56, 0x08, 0x45, 0x12, 0x25, 0xB4, 0xA9,
        0x8F, 0xB2, 0x32, 0x2E, 0xC5, 0x58, 0x62, 0x97, 0xE2, 0x94,
        0xA8, 0x62, 0x3B, 0xBF, 0x3F, 0x75, 0xDC, 0xDD, 0xCF, 0x12,
        0x34, 0x75, 0xE7, 0x51, 0x46, 0xAB, 0xBF, 0xF5, 0x20, 0xEB,
        0x22, 0xCE, 0x8F, 0x71, 0xFE, 0xF3, 0x52, 0x35, 0x9E, 0x8B,
        0x27, 0xC7, 0xC4, 0x17, 0xC9, 0x0D, 0x8C, 0x27, 0x38, 0x5B,
        0x26, 0x95, 0xA7, 0xB2, 0xE8, 0x42, 0xA4, 0x48, 0x94, 0x8C,
        0xB2, 0x2D, 0x8D, 0xBB, 0xEC, 0x9F, 0xFC, 0x48, 0x6E, 0x66,
        0x01, 0xB8, 0xD8, 0xC8, 0x3C, 0x5A, 0xFD, 0x79, 0xC3, 0x08,
        0xA4, 0x58, 0xB5, 0xFC, 0x6F, 0x7E, 0xF9, 0xFD, 0x61, 0xB0,
        0x49, 0x4B, 0x28, 0x2A, 0xC1, 0x9A, 0x81, 0x84, 0x2D, 0x26,
        0xF4, 0xBC, 0x43, 0xC7, 0x6B, 0x0B
    };
    static unsigned char dhg_2048[] = {
            0x02
    };
    DH *dh = (*(cryptoFuncs->dh_new)) ();
    BIGNUM *p, *g;

    if (dh == NULL)
        return NULL;
    p = (*(cryptoFuncs->bin2bn)) (dhp_2048, sizeof (dhp_2048), NULL);
    g = (*(cryptoFuncs->bin2bn)) (dhg_2048, sizeof (dhg_2048), NULL);
    if (p == NULL || g == NULL || !(*(cryptoFuncs->dh_set_pqg)) (dh, p, NULL, g))
    {
        (*(cryptoFuncs->dh_free)) (dh);
        (*(cryptoFuncs->bn_free)) (p);
        (*(cryptoFuncs->bn_free)) (g);
        return NULL;
    }
    return dh;
}

/*
static unsigned char dh4096_p[] =
{

};
static unsigned char dh4096_g[] =
{
    0x02,
};

static DH *get_dh4096()
{
    DH *dh;

    if ((dh = (*(cryptoFuncs->dh_new))()) == NULL)
        return (NULL);
    dh->p = (*(cryptoFuncs->bin2bn)) (dh1024_p, sizeof(dh1024_p), NULL);
    dh->g = (*(cryptoFuncs->bin2bn)) (dh1024_g, sizeof(dh1024_g), NULL);
    if ((dh->p == NULL) || (dh->g == NULL))
	{
		(*(cryptoFuncs->dh_free))(dh);
        return (NULL);
	}
    return (dh);
}


static unsigned char dh8192_p[] =
{

};
static unsigned char dh8192_g[] =
{
    0x02,
};

static DH *get_dh8192()
{
    DH *dh;

    if ((dh = (*(cryptoFuncs->dh_new))()) == NULL)
        return (NULL);
    dh->p = (*(cryptoFuncs->bin2bn)) (dh1024_p, sizeof(dh1024_p), NULL);
    dh->g = (*(cryptoFuncs->bin2bn)) (dh1024_g, sizeof(dh1024_g), NULL);
    if ((dh->p == NULL) || (dh->g == NULL))
	{
		(*(cryptoFuncs->dh_free))(dh);
        return (NULL);
	}
    return (dh);
}
*/

/* ----END GENERATED SECTION---------- */

DH *ripcSSLDHGetTmpParam(int nKeyLen, ripcSSLBIOApiFuncs* bioFuncs, ripcCryptoApiFuncs* cryptoFuncs)
{
    DH *dh;

    if (nKeyLen == 512)
        dh = get_dh512(cryptoFuncs);
    else if (nKeyLen == 1024)
        dh = get_dh1024(cryptoFuncs);
    else if (nKeyLen == 2048)
        dh = get_dh2048(cryptoFuncs);
    else
        dh = get_dh2048(cryptoFuncs);
    return dh;
}

DH *ripcSSLDHGetParamFile( char *file, ripcSSLBIOApiFuncs* bioFuncs, ripcCryptoApiFuncs* cryptoFuncs)
{
    DH *dh = NULL;
    BIO *bio;

    if ((bio = (*(bioFuncs->new_file))(file, "r")) == NULL)
        return NULL;
    dh = (*(cryptoFuncs->read_bio_dhparams))(bio, NULL, NULL, NULL);
    (*(bioFuncs->bio_free))(bio);
    return (dh);
}

/*
=cut
##
##  Embedded Perl script for generating the temporary DH parameters
##

require 5.003;
use strict;

#   configuration
my $file  = $0;
my $begin = '----BEGIN GENERATED SECTION--------';
my $end   = '----END GENERATED SECTION----------';

#   read ourself and keep a backup
open(FP, "<$file") || die;
my $source = '';
$source .= $_ while (<FP>);
close(FP);
open(FP, ">$file.bak") || die;
print FP $source;
close(FP);

#   generate the DH parameters
print "1. Generate 512 and 1024 bit Diffie-Hellman parameters (p, g)\n";
my $rand = '';
foreach $file (qw(/var/log/messages /var/adm/messages 
                  /kernel /vmunix /vmlinuz /etc/hosts /etc/resolv.conf)) {
    if (-f $file) {
        $rand = $file     if ($rand eq '');
        $rand .= ":$file" if ($rand ne '');
    }
}
$rand = "-rand $rand" if ($rand ne '');
system("openssl dhparam $rand -out dh512.pem 512");
system("openssl dhparam $rand -out dh1024.pem 1024");

#   generate DH param info 
my $dhinfo = '';
open(FP, "openssl dhparam -noout -text -in dh512.pem |") || die;
$dhinfo .= $_ while (<FP>);
close(FP);
open(FP, "openssl dhparam -noout -text -in dh1024.pem |") || die;
$dhinfo .= $_ while (<FP>);
close(FP);
$dhinfo =~ s|^|** |mg;
$dhinfo = "\n\/\*\n$dhinfo\*\/\n\n";

#   generate C source from DH params
my $dhsource = '';
open(FP, "openssl dhparam -noout -C -in dh512.pem | indent | expand -8 |") || die;
$dhsource .= $_ while (<FP>);
close(FP);
open(FP, "openssl dhparam -noout -C -in dh1024.pem | indent | expand -8 |") || die;
$dhsource .= $_ while (<FP>);
close(FP);
$dhsource =~ s|(DH\s+\*get_dh)|static $1|sg;

#   generate output
my $o = $dhinfo . $dhsource;

#   insert the generated code at the target location
$source =~ s|(\/\* $begin.+?\n).*\n(.*?\/\* $end)|$1$o$2|s;

#   and update the source on disk
print "Updating file `$file'\n";
open(FP, ">$file") || die;
print FP $source;
close(FP);

#   cleanup
unlink("dh512.pem");
unlink("dh1024.pem");

=pod
*/

#endif
