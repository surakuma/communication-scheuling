#!/usr/bin/env Rscript
insights_func <-function(file_name)
{
library(reshape2)
dftest<-read.table(file_name, header=T)


dftest[,1:ncol(dftest)]<-dftest[,1:ncol(dftest)]/dftest[,5]
dftest[,5]<-NULL
dftestMelted<-melt(dftest)
names(dftestMelted)<-c( "Parameter", "ratio_to_optimal")
p<-ggplot(dftestMelted, aes(x=Parameter, y=ratio_to_optimal, color=Parameter))  + geom_boxplot(outlier.color="black") + geom_hline(yintercept=1, linetype="dashed", color = "red")
p <- p + theme(plot.title = element_text(hjust = 0.5), legend.position='none') +  ggtitle("Application Properties") + scale_y_continuous(breaks=seq(0.0, 2.0, 0.5), limits=c(0, 2))
#p <- p + theme(plot.title = element_text(hjust = 0.5), legend.position='none') +  ggtitle("Application Properties") 

return (p)
}
#p<-insights_func('insight_hf.txt')
#w=20
#h=10
#ggsave(file="application_properties_hf.pdf", p, width=w, height=h)
#
#p<-insights_func('insight_ccsd.txt')
#ggsave(file="application_properties_ccsd.pdf", p, width=w, height=h)

get_melted_data_frame<-function(file_name)
{
library(reshape2)
dftest<-read.table(file_name, header=T)
colnames(dftest)[colnames(dftest) == "sum_comm" ] <- "sum comm"
colnames(dftest)[colnames(dftest) == "sum_comp" ] <- "sum comp"
colnames(dftest)[colnames(dftest) == "sum_comm_comp" ] <- "sum comm + sum comp"
colnames(dftest)[colnames(dftest) == "max.sum_comm.sum_comp." ] <- "max(sum comm, sum comp)"
dftest[,1:ncol(dftest)]<-dftest[,1:ncol(dftest)]/dftest[,5]
dftest[,5]<-NULL
dftest<-dftest[,c(3,4,2,1)]
dftestMelted<-melt(dftest)
names(dftestMelted)<-c( "Parameter", "ratio_to_optimal")
return (dftestMelted)
}
dfMelted1<-get_melted_data_frame('insight_hf.txt');
dfMelted1$application <- "HF Workload"
dfMelted2<-get_melted_data_frame('insight_ccsd.txt');
dfMelted2$application <- "CCSD Workload"
dfMelted <- rbind(dfMelted1, dfMelted2)
library(ggplot2)
p<-ggplot(dfMelted, aes(x=Parameter, y=ratio_to_optimal, color=Parameter))  + geom_boxplot(outlier.color="black") + geom_hline(yintercept=1, linetype="dashed", color = "red") +
          facet_wrap(~application) + scale_y_continuous(breaks=seq(0.0, 2.0, 0.5), limits=c(0, 2)) +
          theme(legend.position='none', axis.text.x = element_text(angle = 25, hjust=1, vjust = 1)) + xlab(" ") +
          ylab("Ratio to Optimal")
ggsave(file="application_properties.pdf", p, width=7, height=4)

