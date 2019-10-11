
/**
 *  \addtogroup CkLdb
 * Giovane Lizot
 */

/*@{*/

#include "GroupLB.h"
#include "ckgraph.h"

//const double macro = 012.345;
double aux = 0.0;

CreateLBFunc_Def(GroupLB, "My first load balancer")

GroupLB::GroupLB(const CkLBOptions &opt) : CentralLB(opt) {
	lbname = "GroupLB";
	if(CkMyPe() == 0)
		CkPrintf("GroupLB created on processor %d\n",CkMyPe());
}

CmiBool GroupLB::QueryBalanceNow(int _step) {
	return CmiTrue;
}

int peMenorQueMedia(int npes, double *pe_loads, double mag){

	int indice = 0;


	for (int i = 0; i < npes; i++){

		if(pe_loads[i] < mag){

			indice = i;
        //CkPrintf("Menor PE encontrado = %d\n", i);
			break;

		}

	}

	return indice;

}

int nmoves = 0, ctrl = 0;

void GroupLB::procuraProc(LDStats *stats, double *pe_loads, double *cmp, double mag, double walltime) {


	int menorPe = peMenorQueMedia(stats->nprocs(), pe_loads, mag);
    //const double macro = 012.345;
    //CkPrintf("menorPe = %d\n", menorPe);
	CkPrintf("MAG = %f\n", mag);

	for (int i=0; i<stats->nprocs(); i++) {

		if(pe_loads[i] > mag) {
	    //CkPrintf("Processador maior que a média encontrado: %d\n", i);

			for (int j=0; j < stats-> n_objs; j++) {

				int cpe = stats->from_proc[j];

				LDObjData &oData = stats->objData[j];
                    //CkPrintf("PE ATUAL = %d\n", cpe);
				walltime = oData.wallTime;

                    //ESTRATÉGIA
                    if(walltime > cmp[i] ){ // Se a carga do objeto for maior que a carga média do proc

                    	aux = pe_loads[menorPe];
                    	if((aux + walltime) > mag) continue;

			//CkPrintf("O Obj %d possui walltime de %f\n", j, walltime);
                        int dest = menorPe; //Pe destino = Pe menos carregado
                        const int pe   = i; //Pe atual
                        const int id   = j; //id do objeto


			     //se a carga do menorPe ainda estiver menor que a média
		                //CkPrintf("Migrando objeto %d com carga %f para o PE %d\n", id, walltime, dest);
                                stats->to_proc[id] = dest; //migra objeto para o Pe que está abaixo da média
                                pe_loads[menorPe] += walltime;
                                pe_loads[i] -= walltime;

			        //CkPrintf("pe_loads[%d] = %f\n", menorPe, pe_loads[menorPe]);
                                nmoves ++;  //incrementa num de migrações
                                //CkPrintf("[%d] Obj %d migrating from %d to %d\n", CkMyPe(),j,pe,dest);
                                LDObjData &oData = stats->objData[j+1];
                                double walltimeaux = oData.wallTime;

                                if(pe_loads[menorPe] > mag) {
			        //CkPrintf("Menor PE deixou de ser menor que a média\n");

				//CkPrintf("Escolhendo outro Pe para migrar..\n");
                                	menorPe = peMenorQueMedia(stats->nprocs(), pe_loads, mag);

                                }

                            }

                        }
                    }
                    CkPrintf("pe_loads_balanced[%d] = %f\n", i, pe_loads[i]);
                }
            }




            void GroupLB::work(LDStats *stats) {
  /** ========================== INITIALIZATION ============================= */

  int n_pes = stats->nprocs(), nmoves = 0, maiorPe = 0, menorPe = 0; //n_pes = numero de processadores, nmoves = numero de migrações
  int num_objs_pe[n_pes];
  double pe_time = 0.0, load = 0.0, mag = 0.0;//pe_time = tempo de cada Pe, load = carga média
  double pe_loads[n_pes], cmp[n_pes]; //carga total de cada Pe
  //ProcArray *parr = new ProcArray(stats);

  //Preparing Strategy

  for (int i=0; i<n_pes; i++) {

  	pe_loads[i] = 0;
  	num_objs_pe[i] = 0;

  }

  double walltime = 0.0; //tempo total
  const int obj = 1;
  for(int i = 0; i < stats->n_objs; i++) {

  	LDObjData &oData = stats->objData[i];
      int pe = stats->from_proc[i]; //Pe atual
      walltime = oData.wallTime; //tempo total do objeto
      //CkPrintf("O obj %d no PE %d possui o tempo de %f\n",i, pe, walltime);
      pe_loads[pe] += walltime; //calcula a carga total de cada Pe
      num_objs_pe[pe] += obj;
      //CkPrintf("pe_loads[%d] += %f\n",pe, pe_loads[pe]);
      //load += walltime; //calcula a carga total de todos os objetos

  }

  //load = load/stats->n_objs; //media aritmética da carga = carga total / num_objetos
  //double avg_load = 0;
  for (int i=0; i<n_pes; i++) {

      //load += pe_loads[i];
  	CkPrintf("pe_loads[%d] += %f\n",i, pe_loads[i]);
      //avg_load += pe_loads[i];
  	cmp[i] = pe_loads[i]/num_objs_pe[i];
  	mag += pe_loads[i];
      //CkPrintf("O PE %d possui carga média de %f\n",i, pe_avg_loads[i]);
  }

  mag /= n_pes;
  //CkPrintf("A Carga Média é de %f\n", avg_load);

  //maiorPe = peMaisCarregado(n_pes, pe_loads);
  //CkPrintf("O PE mais carregado possui carga de %d\n", maiorPe);
  //menorPe = peMenosCarregado(n_pes, pe_loads);
  //CkPrintf("O PE menos carregado possui carga de %d\n", menorPe);


  /** ============================= STRATEGY ================================ */

  procuraProc(stats, pe_loads, cmp, mag, walltime);

  /** ============================== CLEANUP ================================ */
}


#include "GroupLB.def.h"

/*@}*/
