
# Orthus

Linux shell deamon created to synchronize two directories with each other




## Deployment

To deploy Orthus run the following command:

```bash
  orthus -s Source/Dir/Path - t Target/Dir/Path
```
Where Source/Dir/Path is a path to your source directory, and Target/Dir/Path is a path to your target directory
To set the frequency of daemon waking up, use the -f flag:

```bash
  orthus -s ./a - t ./b -f 5
```
followed by the frequency expressed in minutes
To see available options run:
```bash
  orthus -h
```
To make the daemon copy directories present in the source directory as well (Recursively) run:
```bash
  orthus -s ./a -t ./b -R
```

To force synchronization from LINUX terminal run 
```bash
  kill -USR1 1234
```
while replacing '1234' with number of proces. You may find the number of proces by running
```bash
  journalctl -f
```
To end orthus kill it with bash 'kill' command
