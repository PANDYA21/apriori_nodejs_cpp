library(ggplot2)
library(jsonlite)

benchmark_trans <- fromJSON('./benchmark_trans2.json');
benchmark_items <- fromJSON('./benchmark_items2.json');

# print(benchmark_trans)

plt_trans <- ggplot(data = benchmark_trans) +
  geom_boxplot(mapping = aes(x = factor(nTrans), y = jsTime, fill = 'js')) +
  geom_boxplot(mapping = aes(x = factor(nTrans), y = cppTime, fill = 'cpp')) + 
  xlab('number of transactions') + 
  ylab('mining duration (ms)') + 
  scale_y_continuous(trans = 'log10', breaks = c(seq(2e3, 5e3, by = 1e3), seq(4e4, 1e7, by = 1e4))) +
  theme(axis.ticks.y = element_line())

plt_items <- ggplot(data = benchmark_items) +
  geom_boxplot(mapping = aes(x = factor(nItems), y = jsTime, fill = 'js')) +
  geom_boxplot(mapping = aes(x = factor(nItems), y = cppTime, fill = 'cpp')) + 
  xlab('number of unique items in itemSet') + 
  ylab('mining duration (ms)') + 
  scale_y_continuous(trans = 'log10', breaks = c(seq(2e3, 5e3, by = 1e3), seq(4e4, 1e7, by = 1e4))) +
  theme(axis.ticks.y = element_line())

ggsave('./plt_trans.pdf', plt_trans);
ggsave('./plt_items.pdf', plt_items);
