/* CloneBuster
	  
File:     ThreadTest.cpp
Authors:  Samira Briongos (samira.briongos@neclab.eu)
            

NEC Laboratories Europe GmbH, Copyright (c) 2023, All rights reserved.  

       THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 
       PROPRIETARY INFORMATION ---  

SOFTWARE LICENSE AGREEMENT

ACADEMIC OR NON-PROFIT ORGANIZATION NONCOMMERCIAL RESEARCH USE ONLY

BY USING OR DOWNLOADING THE SOFTWARE, YOU ARE AGREEING TO THE TERMS OF THIS
LICENSE AGREEMENT.  IF YOU DO NOT AGREE WITH THESE TERMS, YOU MAY NOT USE OR
DOWNLOAD THE SOFTWARE.

This is a license agreement ("Agreement") between your academic institution
or non-profit organization or self (called "Licensee" or "You" in this
Agreement) and NEC Laboratories Europe GmbH (called "Licensor" in this
Agreement).  All rights not specifically granted to you in this Agreement
are reserved for Licensor. 

RESERVATION OF OWNERSHIP AND GRANT OF LICENSE: Licensor retains exclusive
ownership of any copy of the Software (as defined below) licensed under this
Agreement and hereby grants to Licensee a personal, non-exclusive,
non-transferable license to use the Software for noncommercial research
purposes, without the right to sublicense, pursuant to the terms and
conditions of this Agreement. NO EXPRESS OR IMPLIED LICENSES TO ANY OF
LICENSOR'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE. As used in this
Agreement, the term "Software" means (i) the actual copy of all or any
portion of code for program routines made accessible to Licensee by Licensor
pursuant to this Agreement, inclusive of backups, updates, and/or merged
copies permitted hereunder or subsequently supplied by Licensor,  including
all or any file structures, programming instructions, user interfaces and
screen formats and sequences as well as any and all documentation and
instructions related to it, and (ii) all or any derivatives and/or
modifications created or made by You to any of the items specified in (i).

CONFIDENTIALITY/PUBLICATIONS: Licensee acknowledges that the Software is
proprietary to Licensor, and as such, Licensee agrees to receive all such
materials and to use the Software only in accordance with the terms of this
Agreement.  Licensee agrees to use reasonable effort to protect the Software
from unauthorized use, reproduction, distribution, or publication. All
publication materials mentioning features or use of this software must
explicitly include an acknowledgement the software was developed by NEC
Laboratories Europe GmbH.

COPYRIGHT: The Software is owned by Licensor.  

PERMITTED USES:  The Software may be used for your own noncommercial
internal research purposes. You understand and agree that Licensor is not
obligated to implement any suggestions and/or feedback you might provide
regarding the Software, but to the extent Licensor does so, you are not
entitled to any compensation related thereto.

DERIVATIVES: You may create derivatives of or make modifications to the
Software, however, You agree that all and any such derivatives and
modifications will be owned by Licensor and become a part of the Software
licensed to You under this Agreement.  You may only use such derivatives and
modifications for your own noncommercial internal research purposes, and you
may not otherwise use, distribute or copy such derivatives and modifications
in violation of this Agreement.

BACKUPS:  If Licensee is an organization, it may make that number of copies
of the Software necessary for internal noncommercial use at a single site
within its organization provided that all information appearing in or on the
original labels, including the copyright and trademark notices are copied
onto the labels of the copies.

USES NOT PERMITTED:  You may not distribute, copy or use the Software except
as explicitly permitted herein. Licensee has not been granted any trademark
license as part of this Agreement.  Neither the name of NEC Laboratories
Europe GmbH nor the names of its contributors may be used to endorse or
promote products derived from this Software without specific prior written
permission.

You may not sell, rent, lease, sublicense, lend, time-share or transfer, in
whole or in part, or provide third parties access to prior or present
versions (or any parts thereof) of the Software.

ASSIGNMENT: You may not assign this Agreement or your rights hereunder
without the prior written consent of Licensor. Any attempted assignment
without such consent shall be null and void.

TERM: The term of the license granted by this Agreement is from Licensee's
acceptance of this Agreement by downloading the Software or by using the
Software until terminated as provided below.  

The Agreement automatically terminates without notice if you fail to comply
with any provision of this Agreement.  Licensee may terminate this Agreement
by ceasing using the Software.  Upon any termination of this Agreement,
Licensee will delete any and all copies of the Software. You agree that all
provisions which operate to protect the proprietary rights of Licensor shall
remain in force should breach occur and that the obligation of
confidentiality described in this Agreement is binding in perpetuity and, as
such, survives the term of the Agreement.

FEE: Provided Licensee abides completely by the terms and conditions of this
Agreement, there is no fee due to Licensor for Licensee's use of the
Software in accordance with this Agreement.

DISCLAIMER OF WARRANTIES:  THE SOFTWARE IS PROVIDED "AS-IS" WITHOUT WARRANTY
OF ANY KIND INCLUDING ANY WARRANTIES OF PERFORMANCE OR MERCHANTABILITY OR
FITNESS FOR A PARTICULAR USE OR PURPOSE OR OF NON- INFRINGEMENT.  LICENSEE
BEARS ALL RISK RELATING TO QUALITY AND PERFORMANCE OF THE SOFTWARE AND
RELATED MATERIALS.

SUPPORT AND MAINTENANCE: No Software support or training by the Licensor is
provided as part of this Agreement.  

EXCLUSIVE REMEDY AND LIMITATION OF LIABILITY: To the maximum extent
permitted under applicable law, Licensor shall not be liable for direct,
indirect, special, incidental, or consequential damages or lost profits
related to Licensee's use of and/or inability to use the Software, even if
Licensor is advised of the possibility of such damage.

EXPORT REGULATION: Licensee agrees to comply with any and all applicable
export control laws, regulations, and/or other laws related to embargoes and
sanction programs administered by law.

SEVERABILITY: If any provision(s) of this Agreement shall be held to be
invalid, illegal, or unenforceable by a court or other tribunal of competent
jurisdiction, the validity, legality and enforceability of the remaining
provisions shall not in any way be affected or impaired thereby.

NO IMPLIED WAIVERS: No failure or delay by Licensor in enforcing any right
or remedy under this Agreement shall be construed as a waiver of any future
or other exercise of such right or remedy by Licensor.

GOVERNING LAW: This Agreement shall be construed and enforced in accordance
with the laws of Germany without reference to conflict of laws principles.
You consent to the personal jurisdiction of the courts of this country and
waive their rights to venue outside of Germany.

ENTIRE AGREEMENT AND AMENDMENTS: This Agreement constitutes the sole and
entire agreement between Licensee and Licensor as to the matter set forth
herein and supersedes any previous agreements, understandings, and
arrangements between the parties relating hereto.

       THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//#include <fcntl.h>
#include "string.h"
#include "mbusafecrt.h"
#include "pthread.h"
#include "sgx_thread.h"
#include "sgx_trts.h"
#include "sgx_tseal.h"
#include "rtm.h"

//#include <immintrin.h> //tsx
//#include <x86intrin.h>

/*For huge pages --> No longer available*/
/*#define FILE_NAME "/mnt/hugetlbfs/filehuge"
#define PROTECTION (PROT_READ | PROT_WRITE)*/
/* Only ia64 requires this */
/*#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define FLAGS (MAP_SHARED | MAP_FIXED)
#else
#define ADDR (void *)(0x0UL)
#define FLAGS (MAP_SHARED)
#endif*/

#define BUFFER_SIZE 50
#define SAMPLES_SIZE 5
#define RESERVED_MEMORY 36*1024*1024
#define RESERVED_RAM 4*1024*1024
#define TEST_SET 28
#define CACHE_SET_SIZE 16
#define CACHE_SLICES 12
#define MONITORED_SETS 16 // All the possible values
#define WAYS_FILLED 16
#define NEXT_SET (1ULL << (6 + 10))
#define MON_WINDOW 16

#define ACK_TLB 0

#define TLB_L1_WAYS 4
#define TLB_L1_SETS 64
#define TLB_L2_WAYS 12
#define TLB_L2_SETS 1536
#define TLB_SIZE 4*64
#define TLB_SIZE_2 1536
#define PAGE_SIZE 4*1024
#define PAGE_BITS 12
//#define EV_SET_SIZE TLB_L1_WAYS+TLB_L2_WAYS
#define EV_SET_SIZE 4
#define RESERVED_MEMORY_TLB (EV_SET_SIZE+4)*PAGE_SIZE//*TLB_SIZE_2

#define SLICE_MASK_0 0x0002040000UL
#define SLICE_MASK_1 0x0001020000UL
#define SLICE_MASK_2 0x0000810000UL
#define SLICE_MASK_3 0x0000408000UL
#define SLICE_MASK_4 0x0000204000UL
#define SLICE_MASK_5 0x0000102000UL
#define SLICE_MASK_6 0x0000081000UL

#define PAGE_COUNT RESERVED_MEMORY/4096
#define WINDOW_SIZE 14
#define SPOILER_SETS 256 //90
#define SPOILER_PEAKS 38 //PAGE_COUNT/125

#define mfence()  __asm__ volatile("mfence;"); 
#define lfence()  __asm__ volatile("lfence;");

extern long int global_counter;    /* global counter */
extern int monitor_running;
extern int monitor_ready;
extern int sampler_ready;
//extern sgx_thread_mutex_t global_mutex;
extern pthread_mutex_t global_mutex; 
extern pthread_mutex_t mutex_start;
extern pthread_cond_t condp;
extern pthread_cond_t condSets;
extern unsigned char secret_data[BUFFER_SIZE];
extern char memory_map[RESERVED_MEMORY];
extern long int eviction_set[MONITORED_SETS*CACHE_SET_SIZE*CACHE_SLICES];
extern long int eviction_set_1[CACHE_SET_SIZE*CACHE_SLICES];
extern long int final_eviction_set[MONITORED_SETS*CACHE_SET_SIZE*CACHE_SLICES];
extern long int eviction_add[MONITORED_SETS*CACHE_SLICES];
extern long int eviction_add_1[CACHE_SLICES];
extern int ram_border;
extern uint8_t *secret_data_read;
extern long int *base_address;
extern int secret_set;
extern int fd;
extern char memory_map_tlb[RESERVED_MEMORY_TLB];
extern long int eviction_set_tlb[EV_SET_SIZE];
extern long int samples_data[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES * SAMPLES_SIZE];
extern long int samples_tlb[EV_SET_SIZE * SAMPLES_SIZE];
extern size_t samples_rip[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES * SAMPLES_SIZE];
//extern int samples_exit[MONITORED_SETS * WAYS_FILLED * CACHE_SLICES * SAMPLES_SIZE];
extern long int samples_time[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES * SAMPLES_SIZE];
extern long int measurements[PAGE_COUNT];
extern long int array_addresses[SPOILER_SETS][SPOILER_PEAKS];
extern long int set_addresses[SPOILER_SETS * SPOILER_PEAKS];
extern int mon_ways;

/*
 * increase_counter:
 *   Utilize thread APIs inside the enclave.
 */
long int mem_access(long int *v);
void flush_data(long int *pos_data);
void flush_desired_set(long int address_array[], int len);
long int access_timed(long int *pos_data,long int *t1);
long int access_timed_1(long int *pos_data);
long int calibrate_miss(long int *pos_data);
int probe_candidate(int number_elements, long int array_set[], long int *dir_candidate);
int probe_candidate_sorted(int number_elements, long int array_set[], long int *dir_candidate);
int probe_candidate_flush(int number_elements, long int array_set[], long int *dir_candidate);
void randomize_set(long int array[], int len);
int check_inside(long int dato, long int array_data[], int array_length);
long int hammer (long int *pos1, long int *pos2, int rounds);
long int increase_counter();
long int check_counter();
int check_running();
int check_ready();
int check_sampler();
int assign_secret (uint8_t* sealed_blob, uint32_t* data_size);
sgx_status_t unseal_data(const uint8_t *sealed_blob, size_t data_size);
void* counter (void *a);
void* fast_counter (void *);

int parity(unsigned long int v);
int addr2slice_linear(unsigned long int addr, int bitsset);
void test_tlb_l1(int ways, int sets, int rounds);
void test_tlb_l2(int ways, int sets, int rounds);
void build_tlb_eviction(int max, long int array[]);
long int prime_ev_set(unsigned long int pos_data);
long int measure_access(long int *pos_data);
void address_aliasing(int w, int x, int ini, long int meas[PAGE_COUNT]);
int peak_search (long int simple_set[CACHE_SET_SIZE*CACHE_SLICES],long int meas[PAGE_COUNT]);


