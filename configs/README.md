# Configuration Files
These are the configuration files used in the simulation. I choose to use [.yaml](https://www.redhat.com/en/topics/automation/what-is-yaml) files since:
* They are human readable
* They can store relatively complex hierarchical data (SPACECPP doesn't need the full power of XML files)
* Well tested parsers like [yaml-cpp](https://github.com/jbeder/yaml-cpp) already exist to read in the config files

For the wakefield functions, they are stored as simple .csv files with the first column being $\tau$ and the second column being the value of the wakefield. These files can be generated either via [the python notebook](../Notebooks/WakefieldGenration.ipynb), or via some external solver. 

I am making the tacit assumption that the wakefield function is only defined for $\tau>0$ for causality reasons, and that the wakefield function is long enough to support the entire runtime of the simulation. While the first constraint is implemented, it's on the user to ensure the second is met.