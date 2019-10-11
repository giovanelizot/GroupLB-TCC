/**
 *  \addtogroup CkLdb
 */

/*@{*/

#ifndef _GROUPLB_H_
#define _GROUPLB_H_

#include "CentralLB.h"
#include "GroupLB.decl.h"

void CreateGroupLB();

class GroupLB : public CentralLB {
  public:
    GroupLB(const CkLBOptions &opt);
    GroupLB(CkMigrateMessage *m) : CentralLB (m) { };
    void procuraProc(LDStats *stats, double *pe_loads, double *cmp, double mag, double walltime);
    void work(LDStats *stats);
    void pup(PUP::er &p) { CentralLB::pup(p); }
    void FreeProcs(double* bufs);
    void FreeProcs(int* bufs);

  private:
    CmiBool QueryBalanceNow(int _step);
};

#endif /* _GRAPHPARTLB_H_ */

/*@}*/
