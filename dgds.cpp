/*
 * dgds.cpp
 *
 *  Created on: 2015Äê4ÔÂ18ÈÕ
 *      Author: YY_KD
 */

int bob_invocation::get_bobid(int no, char **arg)
{
	int no_sys, no_subsys, no_bob;
	char *sys, *subsys, *bob, *bobin;
	int i, j, k, l, m, n;
	char bob_in_name[4 * MAX_NAME_LEN], bob_in[MAX_NAME_LEN];
	char hostname[34];

	sys = arg[1];
	subsys = arg[2];
	bob = arg[3];
	if (no == 5) {
		bobin = arg[4];
	} else {
		bobin = &bob_in[0];
		strcpy(bobin, "");
	}
	if (no < 4) {
		printf(" expecting 3 parameters  ! retry again !\n");
		abort();
	}

	// get hostname
	i = gethostname(hostname, 34);
	if (i == -1) {
		printf("\n get hostname failed !");
		abort();
	}
	// form the bob_in_name
	strcpy(bob_in_name, sys);
	strcat(bob_in_name, subsys);
	strcat(bob_in_name, bob);
	strcat(bob_in_name, bobin);
	// get system id
	no_sys = (*temsys_p).no_sys;
	for (i = 0; i < no_sys; i++)
	{
		// add 20090618 avoid null record
		if ((*temsys_p).sys[i].system_name[0] == 0)
		{
			no_sys++;
			continue;
		}

		if (strcmp(sys, (*temsys_p).sys[i].system_name) == 0)
		{
			l = i * MAX_SUBSYS;
			no_subsys = (*temsys_p).sys[i].no_subsys;
			for (j = 0; j < no_subsys; j++)
			{
				// add 20090618 avoid null record
				if ((*temsys_p).subsys[l + j].subsys_name[0] == 0)
				{
					no_subsys++;
					continue;
				}

				if (strcmp(subsys, (*temsys_p).subsys[l + j].sub_name) == 0)
				{
					if (strcmp((*temsys_p).subsys[l + j].node_name, hostname)!= 0)
					{
						printf(
								"\nThe sub_name: %s is wrong ! The hostname is %s ! Retry again!\n",
								subsys, hostname);
						abort();
					}
					else
					{
						// no_bob=MAX_BOB;
						no_bob = (*temsys_p).subsys[l + j].no_bob;
						m = i * MAX_SUBSYS * MAX_BOB + j * MAX_BOB;
						for (k = 0; k < no_bob; k++)
						{
							//add 20090616
							if ((*temsys_p).bob[m + k].bob_name[0] == 0)
							{
								no_bob++;
								continue;
							}
							if (strcmp(bob, (*temsys_p).bob[m + k].bob_name) == 0 && strcmp(bob_in_name,(*temsys_p).bob[m + k].bob_instance_name)
											== 0)
							{
								// Setting the "scada" bob synchronizing flag; added by MFY (2001/09)
								if (strcmp(bob, "scada") == 0)
									scada_sync = 1;
								memcpy(this_bid, (*temsys_p).bob[m + k].bob_id,5);
								return (1);
							}
						}

						break;
					}
				}
			}	// end for(j...)
			break;
		}
	}
	//20150414lyq
	for (j = 0; j < no; j++)
	{
		printf("%s", *arg);
		arg++;
	}
	printf(" parameters for this bob instance are wrong ! retry again !");
	abort();
}

