#!/usr/bin/env Rscript
ratio_to_optimal_func <-function(file_name)
{
library(ggplot2)
library(reshape2)
dftest<-read.table(file_name, header=T)
dftest[,2:ncol(dftest)]<-dftest[,2:ncol(dftest)]/dftest[,6]
dftest[,2:6]<-NULL
dftestMelted<-melt(dftest, "capacity")
names(dftestMelted)<-c("capacity", "Heuristic", "ratio_to_optimal")
p<-ggplot(dftestMelted, aes(x=Heuristic, y=ratio_to_optimal, color=Heuristic)) + geom_boxplot(outlier.color="black") +facet_wrap(~capacity)  + theme(axis.ticks = element_blank(), axis.text.x = element_blank()) + xlab("")
p <- p + theme(plot.title = element_text(hjust = 0.5)) +  ggtitle("Ratio to Optimal")
return (p)
}
p<-ratio_to_optimal_func('all_results_hf.txt')
ggsave(file="ratio_to_optimal_hf.pdf", p, width=23.6, height=13.4)

p<-ratio_to_optimal_func('all_results_ccsd.txt')
ggsave(file="ratio_to_optimal_ccsd.pdf", p, width=23.6, height=13.4)
