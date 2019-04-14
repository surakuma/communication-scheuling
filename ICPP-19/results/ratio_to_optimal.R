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
#dftestMelted$mc <- factor(dftestMelted$capacity, labels = c('m_c', "1.125m_c", "1.375m_c", "1.5m_c", "1.625m_c", "1.75m_c", "1.875m_c", "2m_c"))
dftestMelted$mc <- factor(dftestMelted$capacity, labels = c("m[c]", "1.125~m[c]", "1.375~m[c]", "1.5~m[c]", "1.625~m[c]", "1.75~m[c]", "1.875~m[c]", "2~m[c]"))
p<-ggplot(dftestMelted, aes(x=Heuristic, y=ratio_to_optimal, color=Heuristic)) + geom_boxplot(outlier.color="black") +
#facet_wrap(~capacity, ncol=4)  + theme(axis.ticks = element_blank(), axis.text.x = element_blank()) + xlab("") +
facet_wrap(~mc, labeller=label_parsed, ncol=4)  + theme(axis.ticks = element_blank(), axis.text.x = element_blank()) + xlab("") +
ylab("Ratio to Optimal")
#p <- p + theme(plot.title = element_text(hjust = 0.5)) +  ggtitle("Ratio to Optimal")
return (p)
}
#p<-ratio_to_optimal_func('all_results_hf.txt')
#ggsave(file="ratio_to_optimal_hf.pdf", p, width=14.25, height=6.5)
#
#p<-ratio_to_optimal_func('all_results_ccsd.txt')
#ggsave(file="ratio_to_optimal_ccsd.pdf", p, width=14.25, height=6.5)

p<-ratio_to_optimal_func('selected_results_hf.txt')
ggsave(file="ratio_to_optimal_selected_hf.pdf", p, width=14.25, height=6.5)

p<-ratio_to_optimal_func('selected_results_ccsd.txt')
ggsave(file="ratio_to_optimal_selected_ccsd.pdf", p, width=14.25, height=6.5)

#p<-ratio_to_optimal_func('all_results_random_ccsd.txt')
#ggsave(file="ratio_to_optimal_random_ccsd.pdf", p, width=14.25, height=6.5)
#
#p<-ratio_to_optimal_func('selected_results_random_ccsd.txt')
#ggsave(file="ratio_to_optimal_random_selected_ccsd.pdf", p, width=14.25, height=6.5)
