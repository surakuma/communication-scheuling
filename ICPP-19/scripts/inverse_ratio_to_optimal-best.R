#!/usr/bin/env Rscript
inverse_ratio_to_optimal_best_func <-function(file_name)
{
library(ggplot2)
library(reshape2)
dftest<-read.table(file_name, header=T)
dftest[,2:ncol(dftest)]<-dftest[,2:ncol(dftest)]/dftest[,6]

dftest[,8]<-NULL
dftest[,c(2:6)]<-NULL
dftest$static_best<-do.call('pmin', c(dftest[,3:7]))
dftest$dynamic_best<-do.call('pmin', c(dftest[,8:10]))
dftest$static_dynamic_best<-do.call('pmin', c(dftest[,11:13]))
dftest<-dftest[,c(1,2,14:16)]
dftestMelted<-melt(dftest, "capacity")
names(dftestMelted)<-c("capacity", "Heuristic", "ratio_to_optimal")
#p<-ggplot(dftestMelted, aes(x=jitter(capacity), y=1/ratio_to_optimal, color=Heuristic))  + geom_smooth()
p<-ggplot(dftestMelted, aes(x=capacity, y=1/ratio_to_optimal, color=Heuristic))  + geom_smooth(method="loess")
p<-p + ylab("inverse(ratio_to_optimal)") + theme(plot.title = element_text(hjust = 0.5)) +  ggtitle("Performance With Best of all categories")
return (p)
}

p<-inverse_ratio_to_optimal_best_func('all_results_hf.txt')
ggsave(file="inverse_ratio_to_optimal_hf-best.pdf", p, width=23.6, height=13.4)

p<-inverse_ratio_to_optimal_best_func('all_results_ccsd.txt')
ggsave(file="inverse_ratio_to_optimal_ccsd-best.pdf", p, width=23.6, height=13.4)

