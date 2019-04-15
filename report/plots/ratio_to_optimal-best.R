#!/usr/bin/env Rscript
ratio_to_optimal_best_func <-function(file_name)
{
library(ggplot2)
library(reshape2)
dftest<-read.table(file_name, header=T)
dftest[,2:ncol(dftest)]<-dftest[,2:ncol(dftest)]/dftest[,6]

#dftest[,8]<-NULL
dftest[,c(2:6)]<-NULL
dftest$"Best Static"<-do.call('pmin', c(dftest[,3:9]))
dftest$"Best Dynamic" <-do.call('pmin', c(dftest[,10:12]))
dftest$"Best Static Dynamic"<-do.call('pmin', c(dftest[,13:15]))
dftest<-dftest[,c(1,2,16:18)]
dftestMelted<-melt(dftest, "capacity")
names(dftestMelted)<-c("Capacity", "Heuristic", "ratio_to_optimal")
#p<-ggplot(dftestMelted, aes(x=jitter(capacity), y=1/ratio_to_optimal, color=Heuristic))  + geom_smooth()
#p<-ggplot(dftestMelted, aes(x=Capacity, y=1/ratio_to_optimal, color=Heuristic))  + geom_smooth(method="loess")
p<-ggplot(dftestMelted, aes(x=Capacity, y=ratio_to_optimal, color=Heuristic))  + geom_smooth(method="loess") +
geom_point()
#p<-p + ylab("inverse(ratio_to_optimal)") + theme(plot.title = element_text(hjust = 0.5), legend.position="bottom") +  ggtitle("Performance With Best of all categories")
p<-p + ylab("Ratio to Optimal") + xlab ("Memory Capacity") + theme(legend.position="bottom")
return (p)
}

p<-ratio_to_optimal_best_func('all_results_hf.txt')
ggsave(file="ratio_to_optimal_hf-best.pdf", p, width=6.5, height=5)

p<-ratio_to_optimal_best_func('all_results_ccsd.txt')
ggsave(file="ratio_to_optimal_ccsd-best.pdf", p, width=6.5, height=5)

