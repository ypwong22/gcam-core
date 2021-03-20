#PBS -S /bin/bash
#PBS -A ACF-UTK0011
#PBS -l nodes=1:ppn=16,walltime=10:00:00

cd $GCAM_ROOT
cd exe

./gcam.exe -C config_file.xml
