/*
 * Copyright (c) 1992, 1993, 1994, 1995, 1996, 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * OSPF support contributed by Jeffrey Honig (jch@mitchell.cit.cornell.edu)
 */

#ifndef lint
static const char rcsid[] _U_ =
    "@(#) $Header: /tcpdump/master/tcpdump/print-ospf.c,v 1.66 2007-10-08 07:53:21 hannes Exp $ (LBL)";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tcpdump-stdinc.h>

#include <stdio.h>

#include "interface.h"
#include "addrtoname.h"
#include "extract.h"
#include "gmpls.h"

#include "ospf.h"

#include "ip.h"

static struct tok ospf_option_values[] = {
        { OSPF_OPTION_T,	"MultiTopology" }, /* draft-ietf-ospf-mt-09 */
	{ OSPF_OPTION_E,	"External" },
	{ OSPF_OPTION_MC,	"Multicast" },
	{ OSPF_OPTION_NP,	"NSSA" },
        { OSPF_OPTION_L,        "LLS" },
	{ OSPF_OPTION_DC,	"Demand Circuit" },
	{ OSPF_OPTION_O,	"Opaque" },
	{ OSPF_OPTION_DN,	"Up/Down" },
	{ 0,			NULL }
};

static struct tok ospf_authtype_values[] = {
	{ OSPF_AUTH_NONE,	"none" },
	{ OSPF_AUTH_SIMPLE,	"simple" },
	{ OSPF_AUTH_MD5,	"MD5" },
	{ 0,			NULL }
};

static struct tok ospf_rla_flag_values[] = {
	{ RLA_FLAG_B,		"ABR" },
	{ RLA_FLAG_E,		"ASBR" },
	{ RLA_FLAG_W1,		"Virtual" },
	{ RLA_FLAG_W2,		"W2" },
	{ 0,			NULL }
};

static struct tok type2str[] = {
	{ OSPF_TYPE_UMD,	"UMD" },
	{ OSPF_TYPE_HELLO,	"Hello" },
	{ OSPF_TYPE_DD,		"Database Description" },
	{ OSPF_TYPE_LS_REQ,	"LS-Request" },
	{ OSPF_TYPE_LS_UPDATE,	"LS-Update" },
	{ OSPF_TYPE_LS_ACK,	"LS-Ack" },
	{ 0,			NULL }
};

static struct tok lsa_values[] = {
	{ LS_TYPE_ROUTER,       "Router" },
	{ LS_TYPE_NETWORK,      "Network" },
	{ LS_TYPE_SUM_IP,       "Summary" },
	{ LS_TYPE_SUM_ABR,      "ASBR Summary" },
	{ LS_TYPE_ASE,          "External" },
	{ LS_TYPE_GROUP,        "Multicast Group" },
	{ LS_TYPE_NSSA,         "NSSA" },
	{ LS_TYPE_OPAQUE_LL,    "Link Local Opaque" },
	{ LS_TYPE_OPAQUE_AL,    "Area Local Opaque" },
	{ LS_TYPE_OPAQUE_DW,    "Domain Wide Opaque" },
	{ 0,			NULL }
};

static struct tok ospf_dd_flag_values[] = {
	{ OSPF_DB_INIT,	        "Init" },
	{ OSPF_DB_MORE,	        "More" },
	{ OSPF_DB_MASTER,	"Master" },
    { OSPF_DB_RESYNC,	"OOBResync" },
	{ 0,			NULL }
};

static struct tok lsa_opaque_values[] = {
	{ LS_OPAQUE_TYPE_TE,    "Traffic Engineering" },
	{ LS_OPAQUE_TYPE_GRACE, "Graceful restart" },
	{ LS_OPAQUE_TYPE_RI,    "Router Information" },
	{ 0,			NULL }
};

static struct tok lsa_opaque_te_tlv_values[] = {
	{ LS_OPAQUE_TE_TLV_ROUTER, "Router Address" },
	{ LS_OPAQUE_TE_TLV_LINK,   "Link" },
	{ 0,			NULL }
};

static struct tok lsa_opaque_te_link_tlv_subtlv_values[] = {
	{ LS_OPAQUE_TE_LINK_SUBTLV_LINK_TYPE,            "Link Type" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_LINK_ID,              "Link ID" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_LOCAL_IP,             "Local Interface IP address" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_REMOTE_IP,            "Remote Interface IP address" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_TE_METRIC,            "Traffic Engineering Metric" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_MAX_BW,               "Maximum Bandwidth" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_MAX_RES_BW,           "Maximum Reservable Bandwidth" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_UNRES_BW,             "Unreserved Bandwidth" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_ADMIN_GROUP,          "Administrative Group" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_LINK_LOCAL_REMOTE_ID, "Link Local/Remote Identifier" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_LINK_PROTECTION_TYPE, "Link Protection Type" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_INTF_SW_CAP_DESCR,    "Interface Switching Capability" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_SHARED_RISK_GROUP,    "Shared Risk Link Group" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_BW_CONSTRAINTS,       "Bandwidth Constraints" },
	{ 0,			NULL }
};

static struct tok lsa_opaque_grace_tlv_values[] = {
	{ LS_OPAQUE_GRACE_TLV_PERIOD,             "Grace Period" },
	{ LS_OPAQUE_GRACE_TLV_REASON,             "Graceful restart Reason" },
	{ LS_OPAQUE_GRACE_TLV_INT_ADDRESS,        "IPv4 interface address" },
	{ 0,		        NULL }
};

static struct tok lsa_opaque_grace_tlv_reason_values[] = {
	{ LS_OPAQUE_GRACE_TLV_REASON_UNKNOWN,     "Unknown" },
	{ LS_OPAQUE_GRACE_TLV_REASON_SW_RESTART,  "Software Restart" },
	{ LS_OPAQUE_GRACE_TLV_REASON_SW_UPGRADE,  "Software Reload/Upgrade" },
	{ LS_OPAQUE_GRACE_TLV_REASON_CP_SWITCH,   "Control Processor Switch" },
	{ 0,		        NULL }
};

static struct tok lsa_opaque_te_tlv_link_type_sub_tlv_values[] = {
	{ LS_OPAQUE_TE_LINK_SUBTLV_LINK_TYPE_PTP, "Point-to-point" },
	{ LS_OPAQUE_TE_LINK_SUBTLV_LINK_TYPE_MA,  "Multi-Access" },
	{ 0,			NULL }
};

static struct tok lsa_opaque_ri_tlv_values[] = {
	{ LS_OPAQUE_RI_TLV_CAP, "Router Capabilities" },
	{ 0,		        NULL }
};

static struct tok lsa_opaque_ri_tlv_cap_values[] = {
	{ 1, "Reserved" },
	{ 2, "Reserved" },
	{ 4, "Reserved" },
	{ 8, "Reserved" },
	{ 16, "graceful restart capable" },
	{ 32, "graceful restart helper" },
	{ 64, "Stub router support" },
	{ 128, "Traffic engineering" },
	{ 256, "p2p over LAN" },
	{ 512, "path computation server" },
	{ 0,		        NULL }
};

static struct tok ospf_lls_tlv_values[] = {
	{ OSPF_LLS_EO,	"Extended Options" },
	{ OSPF_LLS_MD5,	"MD5 Authentication" },
	{ 0,	NULL }
};

static struct tok ospf_lls_eo_options[] = {
	{ OSPF_LLS_EO_LR,	"LSDB resync" },
	{ OSPF_LLS_EO_RS,	"Restart" },
	{ 0,	NULL }
};

static char tstr[] = " [|ospf2]";

#ifdef WIN32
#define inline __inline
#endif /* WIN32 */

static int ospf_print_lshdr(__capability const struct lsa_hdr *);
static packetbody_t ospf_print_lsa(__capability const struct lsa *);
static int ospf_decode_v2(__capability const struct ospfhdr *, packetbody_t);
static int ospf_decode_lls(__capability const struct ospfhdr *, register u_int);

int
ospf_print_grace_lsa (packetbody_t tptr, u_int ls_length) {

    u_int tlv_type, tlv_length;


    while (ls_length > 0) {
        PACKET_HAS_SPACE_OR_TRUNC(tptr, 4);
        if (ls_length < 4) {
            printf("\n\t    Remaining LS length %u < 4", ls_length);
            return -1;
        }
        tlv_type = EXTRACT_16BITS(tptr);
        tlv_length = EXTRACT_16BITS(tptr+2);
        tptr+=4;
        ls_length-=4;
                    
        printf("\n\t    %s TLV (%u), length %u, value: ",
               tok2str(lsa_opaque_grace_tlv_values,"unknown",tlv_type),
               tlv_type,
               tlv_length);

        if (tlv_length > ls_length) {
            printf("\n\t    Bogus length %u > %u", tlv_length,
                   ls_length);
            return -1;
        }

        /* Infinite loop protection. */
        if (tlv_type == 0 || tlv_length ==0) {
            return -1;
        }

        PACKET_HAS_SPACE_OR_TRUNC(tptr, tlv_length);
        switch(tlv_type) {

        case LS_OPAQUE_GRACE_TLV_PERIOD:
            if (tlv_length != 4) {
                printf("\n\t    Bogus length %u != 4", tlv_length);
                return -1;
            }
            printf("%us",EXTRACT_32BITS(tptr));
            break;

        case LS_OPAQUE_GRACE_TLV_REASON:
            if (tlv_length != 1) {
                printf("\n\t    Bogus length %u != 1", tlv_length);
                return -1;
            }
            printf("%s (%u)",
                   tok2str(lsa_opaque_grace_tlv_reason_values, "Unknown", *tptr),
                   *tptr);
            break;

        case LS_OPAQUE_GRACE_TLV_INT_ADDRESS:
            if (tlv_length != 4) {
                printf("\n\t    Bogus length %u != 4", tlv_length);
                return -1;
            }
            printf("%s", ipaddr_string(tptr));
            break;

        default:
            if (vflag <= 1) {
                if(!print_unknown_data(tptr,"\n\t      ",tlv_length))
                    return -1;
            }
            break;

        }
        /* in OSPF everything has to be 32-bit aligned, including TLVs */
        if (tlv_length%4 != 0)
            tlv_length+=4-(tlv_length%4);
        ls_length-=tlv_length;
        tptr+=tlv_length;
    }

    return 0;
trunc:
    return -1;
}

int
ospf_print_te_lsa (packetbody_t tptr, u_int ls_length) {

    u_int tlv_type, tlv_length, subtlv_type, subtlv_length;
    u_int priority_level, te_class, count_srlg;
    union { /* int to float conversion buffer for several subTLVs */
        float f; 
        u_int32_t i;
    } bw;

    while (ls_length != 0) {
        PACKET_HAS_SPACE_OR_TRUNC(tptr, 4);
        if (ls_length < 4) {
            printf("\n\t    Remaining LS length %u < 4", ls_length);
            return -1;
        }
        tlv_type = EXTRACT_16BITS(tptr);
        tlv_length = EXTRACT_16BITS(tptr+2);
        tptr+=4;
        ls_length-=4;
                    
        printf("\n\t    %s TLV (%u), length: %u",
               tok2str(lsa_opaque_te_tlv_values,"unknown",tlv_type),
               tlv_type,
               tlv_length);

        if (tlv_length > ls_length) {
            printf("\n\t    Bogus length %u > %u", tlv_length,
                   ls_length);
            return -1;
        }

        /* Infinite loop protection. */
        if (tlv_type == 0 || tlv_length ==0) {
            return -1;
        }

        switch(tlv_type) {
        case LS_OPAQUE_TE_TLV_LINK:
            while (tlv_length >= sizeof(subtlv_type) + sizeof(subtlv_length)) {
                if (tlv_length < 4) {
                    printf("\n\t    Remaining TLV length %u < 4",
                           tlv_length);
                    return -1;
                }
                PACKET_HAS_SPACE_OR_TRUNC(tptr, 4);
                subtlv_type = EXTRACT_16BITS(tptr);
                subtlv_length = EXTRACT_16BITS(tptr+2);
                tptr+=4;
                tlv_length-=4;
                            
                printf("\n\t      %s subTLV (%u), length: %u",
                       tok2str(lsa_opaque_te_link_tlv_subtlv_values,"unknown",subtlv_type),
                       subtlv_type,
                       subtlv_length);
                            
                PACKET_HAS_SPACE_OR_TRUNC(tptr, subtlv_length);
                switch(subtlv_type) {
                case LS_OPAQUE_TE_LINK_SUBTLV_ADMIN_GROUP:
                    printf(", 0x%08x", EXTRACT_32BITS(tptr));
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_LINK_ID:
                case LS_OPAQUE_TE_LINK_SUBTLV_LINK_LOCAL_REMOTE_ID:
                    printf(", %s (0x%08x)",
                           ipaddr_string(tptr),
                           EXTRACT_32BITS(tptr));
                    if (subtlv_length == 8) /* rfc4203 */
                        printf(", %s (0x%08x)",
                               ipaddr_string(tptr+4),
                               EXTRACT_32BITS(tptr+4));
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_LOCAL_IP:
                case LS_OPAQUE_TE_LINK_SUBTLV_REMOTE_IP:
                    printf(", %s", ipaddr_string(tptr));
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_MAX_BW:
                case LS_OPAQUE_TE_LINK_SUBTLV_MAX_RES_BW:
                    bw.i = EXTRACT_32BITS(tptr);
                    printf(", %.3f Mbps", bw.f*8/1000000 );
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_UNRES_BW:
                    for (te_class = 0; te_class < 8; te_class++) {
                        bw.i = EXTRACT_32BITS(tptr+te_class*4);
                        printf("\n\t\tTE-Class %u: %.3f Mbps",
                               te_class,
                               bw.f*8/1000000 );
                    }
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_BW_CONSTRAINTS:
                    printf("\n\t\tBandwidth Constraints Model ID: %s (%u)",
                           tok2str(diffserv_te_bc_values, "unknown", *tptr),
                           *tptr);
                    /* decode BCs until the subTLV ends */
                    for (te_class = 0; te_class < (subtlv_length-4)/4; te_class++) {
                        bw.i = EXTRACT_32BITS(tptr+4+te_class*4);
                        printf("\n\t\t  Bandwidth constraint CT%u: %.3f Mbps",
                               te_class,
                               bw.f*8/1000000 );
                    }
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_TE_METRIC:
                    printf(", Metric %u", EXTRACT_32BITS(tptr));
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_LINK_PROTECTION_TYPE:
                    printf(", %s, Priority %u",
                           bittok2str(gmpls_link_prot_values, "none", *tptr),
                           *(tptr+1));
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_INTF_SW_CAP_DESCR:
                    printf("\n\t\tInterface Switching Capability: %s",
                           tok2str(gmpls_switch_cap_values, "Unknown", *(tptr)));
                    printf("\n\t\tLSP Encoding: %s\n\t\tMax LSP Bandwidth:",
                           tok2str(gmpls_encoding_values, "Unknown", *(tptr+1)));
                    for (priority_level = 0; priority_level < 8; priority_level++) {
                        bw.i = EXTRACT_32BITS(tptr+4+(priority_level*4));
                        printf("\n\t\t  priority level %d: %.3f Mbps",
                               priority_level,
                               bw.f*8/1000000 );
                    }
                    break;
                case LS_OPAQUE_TE_LINK_SUBTLV_LINK_TYPE:
                    printf(", %s (%u)",
                           tok2str(lsa_opaque_te_tlv_link_type_sub_tlv_values,"unknown",*tptr),
                           *tptr);
                    break;

                case LS_OPAQUE_TE_LINK_SUBTLV_SHARED_RISK_GROUP:
                    count_srlg = subtlv_length / 4;
                    if (count_srlg != 0)
                        printf("\n\t\t  Shared risk group: ");
                    while (count_srlg > 0) {
                        bw.i = EXTRACT_32BITS(tptr);
                        printf("%d",bw.i);
                        tptr+=4;
                        count_srlg--;
                        if (count_srlg > 0)
                            printf(", ");
                    }
                    break;

                default:
                    if (vflag <= 1) {
                        if(!print_unknown_data(tptr,"\n\t\t",subtlv_length))
                            return -1;
                    }
                    break;
                }
                /* in OSPF everything has to be 32-bit aligned, including subTLVs */
                if (subtlv_length%4 != 0)
                    subtlv_length+=4-(subtlv_length%4);
                            
                tlv_length-=subtlv_length;
                tptr+=subtlv_length;
                            
            }
            break;
                        
        case LS_OPAQUE_TE_TLV_ROUTER:
            if (tlv_length < 4) {
                printf("\n\t    TLV length %u < 4", tlv_length);
                return -1;
            }
            PACKET_HAS_SPACE_OR_TRUNC(tptr, 4);
            printf(", %s", ipaddr_string(tptr));
            break;
                        
        default:
            if (vflag <= 1) {
                if(!print_unknown_data(tptr,"\n\t      ",tlv_length))
                    return -1;
            }
            break;
        }
        /* in OSPF everything has to be 32-bit aligned, including TLVs */
        if (tlv_length%4 != 0)
            tlv_length+=4-(tlv_length%4);
        ls_length-=tlv_length;
        tptr+=tlv_length;
    }
    return 0;
trunc:
    return -1;
}


static int
ospf_print_lshdr(__capability const struct lsa_hdr *lshp)
{
        u_int ls_length;

        PACKET_HAS_ELEMENT_OR_TRUNC(lshp, ls_length);
        ls_length = EXTRACT_16BITS(&lshp->ls_length);
        if (ls_length < sizeof(struct lsa_hdr)) {
                printf("\n\t    Bogus length %u < header (%lu)", ls_length,
                    (unsigned long)sizeof(struct lsa_hdr));
                return(-1);
        }

        PACKET_HAS_ELEMENT_OR_TRUNC(lshp, ls_seq);	/* XXX - ls_length check checked this */
	printf("\n\t  Advertising Router %s, seq 0x%08x, age %us, length %u",
	       ipaddr_string(&lshp->ls_router),
	       EXTRACT_32BITS(&lshp->ls_seq),
	       EXTRACT_16BITS(&lshp->ls_age),
               ls_length-(u_int)sizeof(struct lsa_hdr));

	PACKET_HAS_ELEMENT_OR_TRUNC(lshp, ls_type);	/* XXX - ls_length check checked this */
        switch (lshp->ls_type) {
	/* the LSA header for opaque LSAs was slightly changed */
        case LS_TYPE_OPAQUE_LL:
        case LS_TYPE_OPAQUE_AL:
        case LS_TYPE_OPAQUE_DW:
            printf("\n\t    %s LSA (%d), Opaque-Type %s LSA (%u), Opaque-ID %u",
                   tok2str(lsa_values,"unknown",lshp->ls_type),
                   lshp->ls_type,

		   tok2str(lsa_opaque_values,
			   "unknown",
			   *(&lshp->un_lsa_id.opaque_field.opaque_type)),
		   *(&lshp->un_lsa_id.opaque_field.opaque_type),
		   EXTRACT_24BITS(&lshp->un_lsa_id.opaque_field.opaque_id)
                   
                   );
            break;

	/* all other LSA types use regular style LSA headers */
	default:
            printf("\n\t    %s LSA (%d), LSA-ID: %s",
                   tok2str(lsa_values,"unknown",lshp->ls_type),
                   lshp->ls_type,
                   ipaddr_string(&lshp->un_lsa_id.lsa_id));
            break;
        }

	PACKET_HAS_ELEMENT_OR_TRUNC(lshp, ls_options);	/* XXX - ls_length check checked this */
        printf("\n\t    Options: [%s]", bittok2str(ospf_option_values,"none",lshp->ls_options));

        return (ls_length);
trunc:
	return (-1);
}

/* draft-ietf-ospf-mt-09 */
static struct tok ospf_topology_values[] = {
    { 0, "default " },
    { 1, "multicast " },
    { 2, "management " },
    { 0, NULL }
};

/*
 * Print all the per-topology metrics.
 */
static void
ospf_print_tos_metrics(__capability const union un_tos *tos)
{
    int metric_count;
    int toscount;

    toscount = tos->link.link_tos_count+1;
    metric_count = 0;

    /*
     * All but the first metric contain a valid topology id.
     */
    while (toscount) { 
        printf("\n\t\ttopology %s(%u), metric %u",
               tok2str(ospf_topology_values, "",
                       metric_count ? tos->metrics.tos_type : 0),
               metric_count ? tos->metrics.tos_type : 0,
               EXTRACT_16BITS(&tos->metrics.tos_metric));
        metric_count++;
        tos++;
        toscount--;
    }
}

/*
 * Print a single link state advertisement.  If truncated or if LSA length
 * field is less than the length of the LSA header, return NULl, else
 * return pointer to data past end of LSA.
 */
static packetbody_t
ospf_print_lsa(__capability const struct lsa *lsap)
{
	__capability const u_int8_t *ls_end;
	__capability const struct rlalink *rlp;
	__capability const struct in_addr *ap;
	__capability const struct aslametric *almp;
	__capability const struct mcla *mcp;
	__capability const u_int32_t *lp;
	register int j, tlv_type, tlv_length, topology;
	register int ls_length;
	packetbody_t tptr;

	tptr = (packetbody_t)lsap->lsa_un.un_unknown; /* squelch compiler warnings */
        ls_length = ospf_print_lshdr(&lsap->ls_hdr);
        if (ls_length == -1)
                return(NULL);
	ls_end = (packetbody_t)lsap + ls_length;
	ls_length -= sizeof(struct lsa_hdr);

	switch (lsap->ls_hdr.ls_type) {

	case LS_TYPE_ROUTER:
		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_rla.rla_flags);
                printf("\n\t    Router LSA Options: [%s]", bittok2str(ospf_rla_flag_values,"none",lsap->lsa_un.un_rla.rla_flags));

		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_rla.rla_count);
		j = EXTRACT_16BITS(&lsap->lsa_un.un_rla.rla_count);
		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_rla.rla_link);
		rlp = lsap->lsa_un.un_rla.rla_link;
		while (j--) {
			PACKET_HAS_ONE_OR_TRUNC(rlp);
			switch (rlp->un_tos.link.link_type) {

			case RLA_TYPE_VIRTUAL:
				printf("\n\t      Virtual Link: Neighbor Router-ID: %s, Interface Address: %s",
				    ipaddr_string(&rlp->link_id),
				    ipaddr_string(&rlp->link_data)); 
                                break;

			case RLA_TYPE_ROUTER:
				printf("\n\t      Neighbor Router-ID: %s, Interface Address: %s",
				    ipaddr_string(&rlp->link_id),
				    ipaddr_string(&rlp->link_data));
				break;

			case RLA_TYPE_TRANSIT:
				printf("\n\t      Neighbor Network-ID: %s, Interface Address: %s",
				    ipaddr_string(&rlp->link_id),
				    ipaddr_string(&rlp->link_data));
				break;

			case RLA_TYPE_STUB:
				printf("\n\t      Stub Network: %s, Mask: %s",
				    ipaddr_string(&rlp->link_id),
				    ipaddr_string(&rlp->link_data));
				break;

			default:
				printf("\n\t      Unknown Router Link Type (%u)",
				    rlp->un_tos.link.link_type);
				return (ls_end);
			}

                        ospf_print_tos_metrics(&rlp->un_tos);

			rlp = (__capability struct rlalink *)((packetbody_t)(rlp + 1) +
			    ((rlp->un_tos.link.link_tos_count) * sizeof(union un_tos)));
		}
		break;

	case LS_TYPE_NETWORK:
		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_nla.nla_mask);
		printf("\n\t    Mask %s\n\t    Connected Routers:",
		    ipaddr_string(&lsap->lsa_un.un_nla.nla_mask));
		ap = lsap->lsa_un.un_nla.nla_router;
		while ((u_char *)ap < ls_end) {
			PACKET_HAS_ONE_OR_TRUNC(ap);
			printf("\n\t      %s", ipaddr_string(ap));
			++ap;
		}
		break;

	case LS_TYPE_SUM_IP:
		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_nla.nla_mask);
		printf("\n\t    Mask %s",
		    ipaddr_string(&lsap->lsa_un.un_sla.sla_mask));
		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_sla.sla_tosmetric);
		lp = lsap->lsa_un.un_sla.sla_tosmetric;
		while ((u_char *)lp < ls_end) {
			register u_int32_t ul;

			PACKET_HAS_ONE_OR_TRUNC(lp);
			ul = EXTRACT_32BITS(lp);
                        topology = (ul & SLA_MASK_TOS) >> SLA_SHIFT_TOS;
			printf("\n\t\ttopology %s(%u) metric %d",
                               tok2str(ospf_topology_values, "", topology),
                               topology,
                               ul & SLA_MASK_METRIC);
			++lp;
		}
		break;

	case LS_TYPE_SUM_ABR:
		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_sla.sla_tosmetric);
		lp = lsap->lsa_un.un_sla.sla_tosmetric;
		while ((u_char *)lp < ls_end) {
			register u_int32_t ul;

			PACKET_HAS_ONE_OR_TRUNC(lp);
			ul = EXTRACT_32BITS(lp);
                        topology = (ul & SLA_MASK_TOS) >> SLA_SHIFT_TOS;
			printf("\n\t\ttopology %s(%u) metric %d",
                               tok2str(ospf_topology_values, "", topology),
                               topology,
                               ul & SLA_MASK_METRIC);
			++lp;
		}
		break;

	case LS_TYPE_ASE:
        case LS_TYPE_NSSA: /* fall through - those LSAs share the same format */
		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_nla.nla_mask);
		printf("\n\t    Mask %s",
		    ipaddr_string(&lsap->lsa_un.un_asla.asla_mask));

		PACKET_HAS_ELEMENT_OR_TRUNC(lsap, lsa_un.un_sla.sla_tosmetric);
		almp = lsap->lsa_un.un_asla.asla_metric;
		while ((u_char *)almp < ls_end) {
			register u_int32_t ul;

			PACKET_HAS_ELEMENT_OR_TRUNC(almp, asla_tosmetric);
			ul = EXTRACT_32BITS(&almp->asla_tosmetric);
                        topology = ((ul & ASLA_MASK_TOS) >> ASLA_SHIFT_TOS);
			printf("\n\t\ttopology %s(%u), type %d, metric",
                               tok2str(ospf_topology_values, "", topology),
                               topology,
                               (ul & ASLA_FLAG_EXTERNAL) ? 2 : 1);
                        if ((ul & ASLA_MASK_METRIC)==0xffffff)
                            printf(" infinite");
                        else
                            printf(" %d", (ul & ASLA_MASK_METRIC));

			PACKET_HAS_ELEMENT_OR_TRUNC(almp, asla_forward);
			if (almp->asla_forward.s_addr) {
				printf(", forward %s",
				    ipaddr_string(&almp->asla_forward));
			}
			PACKET_HAS_ELEMENT_OR_TRUNC(almp, asla_tag);
			if (almp->asla_tag.s_addr) {
				printf(", tag %s",
				    ipaddr_string(&almp->asla_tag));
			}
			++almp;
		}
		break;

	case LS_TYPE_GROUP:
		/* Multicast extensions as of 23 July 1991 */
		mcp = lsap->lsa_un.un_mcla;
		while ((u_char *)mcp < ls_end) {
			PACKET_HAS_ELEMENT_OR_TRUNC(mcp, mcla_vid);
			switch (EXTRACT_32BITS(&mcp->mcla_vtype)) {

			case MCLA_VERTEX_ROUTER:
				printf("\n\t    Router Router-ID %s",
				    ipaddr_string(&mcp->mcla_vid));
				break;

			case MCLA_VERTEX_NETWORK:
				printf("\n\t    Network Designated Router %s",
				    ipaddr_string(&mcp->mcla_vid));
				break;

			default:
				printf("\n\t    unknown VertexType (%u)",
				    EXTRACT_32BITS(&mcp->mcla_vtype));
				break;
			}
		++mcp;
		}
		break;

	case LS_TYPE_OPAQUE_LL: /* fall through */
	case LS_TYPE_OPAQUE_AL: 
	case LS_TYPE_OPAQUE_DW:

	    switch (*(&lsap->ls_hdr.un_lsa_id.opaque_field.opaque_type)) {
            case LS_OPAQUE_TYPE_RI:
		tptr = (packetbody_t)(&lsap->lsa_un.un_ri_tlv.type);

		while (ls_length != 0) {
                    PACKET_HAS_SPACE_OR_TRUNC(tptr, 4);
		    if (ls_length < 4) {
                        printf("\n\t    Remaining LS length %u < 4", ls_length);
                        return(ls_end);
                    }
                    tlv_type = EXTRACT_16BITS(tptr);
                    tlv_length = EXTRACT_16BITS(tptr+2);
                    tptr+=4;
                    ls_length-=4;
                    
                    printf("\n\t    %s TLV (%u), length: %u, value: ",
                           tok2str(lsa_opaque_ri_tlv_values,"unknown",tlv_type),
                           tlv_type,
                           tlv_length);

                    if (tlv_length > ls_length) {
                        printf("\n\t    Bogus length %u > %u", tlv_length,
                            ls_length);
                        return(ls_end);
                    }
                    PACKET_HAS_SPACE_OR_TRUNC(tptr, tlv_length);
                    switch(tlv_type) {

                    case LS_OPAQUE_RI_TLV_CAP:
                        if (tlv_length != 4) {
                            printf("\n\t    Bogus length %u != 4", tlv_length);
                            return(ls_end);
                        }
                        printf("Capabilities: %s",
                               bittok2str(lsa_opaque_ri_tlv_cap_values, "Unknown", EXTRACT_32BITS(tptr)));
                        break;
                    default:
                        if (vflag <= 1) {
                            if(!print_unknown_data(tptr,"\n\t      ",tlv_length))
                                return(ls_end);
                        }
                        break;

                    }
                    tptr+=tlv_length;
                    ls_length-=tlv_length;
                }
                break;

            case LS_OPAQUE_TYPE_GRACE:
                if (ospf_print_grace_lsa((packetbody_t)(&lsap->lsa_un.un_grace_tlv.type),
                                         ls_length) == -1) {
                    return(ls_end);
                }
                break;

	    case LS_OPAQUE_TYPE_TE:
                if (ospf_print_te_lsa((packetbody_t)(&lsap->lsa_un.un_te_lsa_tlv.type),
                                      ls_length) == -1) {
                    return(ls_end);
                }
                break;

            default:
                if (vflag <= 1) {
                    if(!print_unknown_data((packetbody_t)lsap->lsa_un.un_unknown,
                                           "\n\t    ", ls_length))
                        return(ls_end);
                } 
                break;
            }
        }

        /* do we want to see an additionally hexdump ? */
        if (vflag> 1)
            if(!print_unknown_data((packetbody_t)lsap->lsa_un.un_unknown,
                                   "\n\t    ", ls_length)) {
                return(ls_end);
            }
        
	return (ls_end);
trunc:
	return (NULL);
}

static int
ospf_decode_lls(__capability const struct ospfhdr *op,
		register u_int length)
{
    __capability const u_char *dptr;
    __capability const u_char *dataend;
    register u_int length2;
    register u_int16_t lls_type, lls_len;
    register u_int32_t lls_flags;

    switch (op->ospf_type) {

    case OSPF_TYPE_HELLO:
        if (!(op->ospf_hello.hello_options & OSPF_OPTION_L))
            return (0);
        break;

    case OSPF_TYPE_DD:
        if (!(op->ospf_db.db_options & OSPF_OPTION_L))
            return (0);
        break;

    default:
        return (0);
    }

    /* dig deeper if LLS data is available; see RFC4813 */
    length2 = EXTRACT_16BITS(&op->ospf_len);
    dptr = (packetbody_t)op + length2;
    dataend = (packetbody_t)op + length;

    if (EXTRACT_16BITS(&op->ospf_authtype) == OSPF_AUTH_MD5) {
        dptr = dptr + op->ospf_authdata[3];
        length2 += op->ospf_authdata[3];
    }
    if (length2 >= length) {
        printf("\n\t[LLS truncated]");
        return (1);
    }
    PACKET_HAS_SPACE_OR_TRUNC(dptr, 2);
    printf("\n\t  LLS: checksum: 0x%04x", (u_int)EXTRACT_16BITS(dptr));

    dptr += 2;
    PACKET_HAS_SPACE_OR_TRUNC(dptr, 2);
    length2 = EXTRACT_16BITS(dptr);
    printf(", length: %u", length2);

    dptr += 2;
    PACKET_HAS_ONE_OR_TRUNC(dptr);
    while (dptr < dataend) {
        PACKET_HAS_SPACE_OR_TRUNC(dptr, 2);
        lls_type = EXTRACT_16BITS(dptr);
        printf("\n\t    %s (%u)",
               tok2str(ospf_lls_tlv_values,"Unknown TLV",lls_type),
               lls_type);
        dptr += 2;
        PACKET_HAS_SPACE_OR_TRUNC(dptr, 2);
        lls_len = EXTRACT_16BITS(dptr);
        printf(", length: %u", lls_len);
        dptr += 2;
        switch (lls_type) {

        case OSPF_LLS_EO:
            if (lls_len != 4) {
                printf(" [should be 4]");
                lls_len = 4;
            }
            PACKET_HAS_SPACE_OR_TRUNC(dptr, 4);
            lls_flags = EXTRACT_32BITS(dptr);
            printf("\n\t      Options: 0x%08x [%s]", lls_flags,
                   bittok2str(ospf_lls_eo_options,"?",lls_flags));

            break;

        case OSPF_LLS_MD5:
            if (lls_len != 20) {
                printf(" [should be 20]");
                lls_len = 20;
            }
			PACKET_HAS_SPACE_OR_TRUNC(dptr, 4);
            printf("\n\t      Sequence number: 0x%08x", EXTRACT_32BITS(dptr));
            break;
        }

        dptr += lls_len;
    }

    return (0);
trunc:
    return (1);
}

static int
ospf_decode_v2(__capability const struct ospfhdr *op, packetbody_t dataend)
{
	__capability const struct in_addr *ap;
	__capability const struct lsr *lsrp;
	__capability const struct lsa_hdr *lshp;
	__capability const struct lsa *lsap;
	register u_int32_t lsa_count,lsa_count_max;

	switch (op->ospf_type) {

	case OSPF_TYPE_UMD:
		/*
		 * Rob Coltun's special monitoring packets;
		 * do nothing
		 */
		break;

	case OSPF_TYPE_HELLO:
                printf("\n\tOptions [%s]",
                       bittok2str(ospf_option_values,"none",op->ospf_hello.hello_options));

                PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_hello.hello_deadint);
                printf("\n\t  Hello Timer %us, Dead Timer %us, Mask %s, Priority %u",
                       EXTRACT_16BITS(&op->ospf_hello.hello_helloint),
                       EXTRACT_32BITS(&op->ospf_hello.hello_deadint),
                       ipaddr_string(&op->ospf_hello.hello_mask),
                       op->ospf_hello.hello_priority);

		PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_hello.hello_dr);
		if (op->ospf_hello.hello_dr.s_addr != 0)
			printf("\n\t  Designated Router %s",
			    ipaddr_string(&op->ospf_hello.hello_dr));

		PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_hello.hello_bdr);
		if (op->ospf_hello.hello_bdr.s_addr != 0)
			printf(", Backup Designated Router %s",
			    ipaddr_string(&op->ospf_hello.hello_bdr));

                ap = op->ospf_hello.hello_neighbor;
                if ((u_char *)ap < dataend)
                        printf("\n\t  Neighbor List:");
                while ((u_char *)ap < dataend) {
                        PACKET_HAS_ONE_OR_TRUNC(ap);
                        printf("\n\t    %s", ipaddr_string(ap));
                        ++ap;
                }
		break;	/* HELLO */

	case OSPF_TYPE_DD:
		PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_db.db_options);
                printf("\n\tOptions [%s]",
                       bittok2str(ospf_option_values,"none",op->ospf_db.db_options));
		PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_db.db_flags);
                printf(", DD Flags [%s]",
                       bittok2str(ospf_dd_flag_values,"none",op->ospf_db.db_flags));
                PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_db.db_ifmtu);
                if (op->ospf_db.db_ifmtu) {
                        printf(", MTU: %u", EXTRACT_16BITS(&op->ospf_db.db_ifmtu));
                }
                PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_db.db_seq);
                printf(", Sequence: 0x%08x", EXTRACT_32BITS(&op->ospf_db.db_seq));

                /* Print all the LS adv's */
                lshp = op->ospf_db.db_lshdr;
                while (((u_char *)lshp < dataend) && ospf_print_lshdr(lshp) != -1) {
                    ++lshp;
                }
		break;

	case OSPF_TYPE_LS_REQ:
                lsrp = op->ospf_lsr;
                while ((u_char *)lsrp < dataend) {
                    PACKET_HAS_ONE_OR_TRUNC(lsrp);

                    printf("\n\t  Advertising Router: %s, %s LSA (%u)",
                           ipaddr_string(&lsrp->ls_router),
                           tok2str(lsa_values,"unknown",EXTRACT_32BITS(lsrp->ls_type)),
                           EXTRACT_32BITS(&lsrp->ls_type));

                    switch (EXTRACT_32BITS(lsrp->ls_type)) {
                        /* the LSA header for opaque LSAs was slightly changed */
                    case LS_TYPE_OPAQUE_LL:
                    case LS_TYPE_OPAQUE_AL:
                    case LS_TYPE_OPAQUE_DW:
                        printf(", Opaque-Type: %s LSA (%u), Opaque-ID: %u",
                               tok2str(lsa_opaque_values, "unknown",lsrp->un_ls_stateid.opaque_field.opaque_type),
                               lsrp->un_ls_stateid.opaque_field.opaque_type,
                               EXTRACT_24BITS(&lsrp->un_ls_stateid.opaque_field.opaque_id));
                        break;
                    default:
                        printf(", LSA-ID: %s",
                               ipaddr_string(&lsrp->un_ls_stateid.ls_stateid));
                        break;
                    }
                    
                    ++lsrp;
                }
		break;

	case OSPF_TYPE_LS_UPDATE:
                lsap = op->ospf_lsu.lsu_lsa;
                PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_lsu.lsu_count);
                lsa_count_max = EXTRACT_32BITS(&op->ospf_lsu.lsu_count);
                printf(", %d LSA%s",lsa_count_max, PLURAL_SUFFIX(lsa_count_max));
                for (lsa_count=1;lsa_count <= lsa_count_max;lsa_count++) {
                    printf("\n\t  LSA #%u",lsa_count);
                        lsap = (__capability const struct lsa *)ospf_print_lsa(lsap);
                        if (lsap == NULL)
                                goto trunc;
                }
		break;

	case OSPF_TYPE_LS_ACK:
                lshp = op->ospf_lsa.lsa_lshdr;
                while (ospf_print_lshdr(lshp) != -1) {
                    ++lshp;
                }
                break;

	default:
		break;
	}
	return (0);
trunc:
	return (1);
}

void
ospf_print(packetbody_t bp, register u_int length, packetbody_t bp2 _U_)
{
	__capability const struct ospfhdr *op;
	__capability const u_char *dataend;
	const u_char *cp;

	op = (__capability struct ospfhdr *)bp;

        /* XXX Before we do anything else, strip off the MD5 trailer */
	/* XXX-BD: should we truncate the capabilities? */
        PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_authtype);
        if (EXTRACT_16BITS(&op->ospf_authtype) == OSPF_AUTH_MD5) {
                length -= OSPF_AUTH_MD5_LEN;
                snapend = PACKET_SUBTRACT(snapend, OSPF_AUTH_MD5_LEN);
        }

	/* If the type is valid translate it, or just print the type */
	/* value.  If it's not valid, say so and return */
	PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_type);
	cp = tok2str(type2str, "unknown LS-type", op->ospf_type);
	printf("OSPFv%u, %s, length %u",
	       op->ospf_version,
	       cp,
	       length);
	if (*cp == 'u')
		return;

        if(!vflag) { /* non verbose - so lets bail out here */
                return;
        }

	PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_len);
	if (length != EXTRACT_16BITS(&op->ospf_len)) {
		printf(" [len %d]", EXTRACT_16BITS(&op->ospf_len));
	}

	if (length > EXTRACT_16BITS(&op->ospf_len)) {
		dataend = bp + EXTRACT_16BITS(&op->ospf_len);
	} else {
		dataend = bp + length;
	}

	PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_routerid);
        printf("\n\tRouter-ID %s", ipaddr_string(&op->ospf_routerid));

	PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_areaid);
	if (op->ospf_areaid.s_addr != 0)
		printf(", Area %s", ipaddr_string(&op->ospf_areaid));
	else
		printf(", Backbone Area");

	if (vflag) {
		/* Print authentication data (should we really do this?) */
		PACKET_HAS_ELEMENT_OR_TRUNC(op, ospf_authdata);

                printf(", Authentication Type: %s (%u)",
                       tok2str(ospf_authtype_values,"unknown",EXTRACT_16BITS(&op->ospf_authtype)),
                       EXTRACT_16BITS(&op->ospf_authtype));

		switch (EXTRACT_16BITS(&op->ospf_authtype)) {

		case OSPF_AUTH_NONE:
			break;

		case OSPF_AUTH_SIMPLE:
                        printf("\n\tSimple text password: ");
                        safeputs((packetbody_t)op->ospf_authdata, OSPF_AUTH_SIMPLE_LEN);
			break;

		case OSPF_AUTH_MD5:
                        printf("\n\tKey-ID: %u, Auth-Length: %u, Crypto Sequence Number: 0x%08x",
                               *((op->ospf_authdata)+2),
                               *((op->ospf_authdata)+3),
                               EXTRACT_32BITS((op->ospf_authdata)+4));
			break;

		default:
			return;
		}
	}
	/* Do rest according to version.	 */
	switch (op->ospf_version) {

	case 2:
		/* ospf version 2 */
		if (ospf_decode_v2(op, dataend))
			goto trunc;
		if (length > EXTRACT_16BITS(&op->ospf_len)) {
			if (ospf_decode_lls(op, length))
				goto trunc;
		}
		break;

	default:
		printf(" ospf [version %d]", op->ospf_version);
		break;
	}			/* end switch on version */

	return;
trunc:
	fputs(tstr, stdout);
}
