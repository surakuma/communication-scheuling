#!/usr/bin/env Rscript
insights_func <-function(file_name)
{
library(ggplot2)
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
p<-insights_func('insight_hf.txt')
ggsave(file="application_properties_hf.pdf", p, width=23.6, height=13.4)

p<-insights_func('insight_ccsd.txt')
ggsave(file="application_properties_ccsd.pdf", p, width=23.6, height=13.4)


