#!/usr/bin/env Rscript
library(reshape2)
library(ggplot2)
dftest<-read.table('single_trace_results_withlp.txt', header=T)
dftest[,c(2:5)]<-NULL
dftestMelted<-melt(dftest, "capacity")
names(dftestMelted)<-c("capacity", "Heuristic", "makespan")
dftestMelted$mc <- factor(dftestMelted$capacity, labels = c("m[c]", "1.125~m[c]", "1.25~m[c]","1.375~m[c]", "1.5~m[c]", "1.625~m[c]", "1.75~m[c]", "1.875~m[c]", "2~m[c]"))
p <- ggplot(dftestMelted, aes(x=Heuristic, y=makespan, fill=Heuristic)) + geom_bar(stat="identity") + facet_wrap(~mc,
        labeller=label_parsed)  + theme(axis.ticks = element_blank(), axis.text.x = element_text(angle=75, hjust=1)) + xlab("")
ggsave(file="makespan_with_lp.pdf", p,  width=10.5, height=9)
