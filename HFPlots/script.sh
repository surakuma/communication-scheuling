dftest<-read.table('results_all_comm_one_fourth.txt', header=T)
dftest[,2:ncol(dftest)]<-dftest[,2:ncol(dftest)]/dftest[,6]
colnames(dftest)
dftest[5]<-NULL
dftest[5]<-NULL
dftestMelted<-melt(dftest, "capacity")
names(dftestMelted)<-c("capacity", "Heuristic", "ratio_to_optimal")
p<-ggplot(dftestMelted, aes(x=Heuristic, y=ratio_to_optimal, color=Heuristic)) + geom_boxplot(outlier.color="black") +facet_wrap(~capacity)  + theme(axis.ticks = element_blank(), axis.text.x = element_blank()) + xlab("") 
p + theme(plot.title = element_text(hjust = 0.5)) + ggtitle("Comm_one_fourth")
p + theme(plot.title = element_text(hjust = 0.5)) + ggtitle("Communication_one_fourth")
ggsave(file="ratio_to_optimal_comm_one_fourth.pdf")

