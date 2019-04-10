library(ggplot2)
library(jsonlite)

benchmark_trans <- fromJSON('./benchmark_trans2.json');
benchmark_items <- fromJSON('./benchmark_items2.json');

# print(benchmark_trans)

# plt_trans <- ggplot(data = benchmark_trans) +
#   geom_boxplot(mapping = aes(x = factor(nTrans), y = jsTime, fill = 'js')) +
#   geom_boxplot(mapping = aes(x = factor(nTrans), y = cppTime, fill = 'cpp')) + 
#   xlab('number of transactions') + 
#   ylab('mining duration (ms)') + 
#   scale_y_continuous(trans = 'log10', breaks = c(seq(2e3, 5e3, by = 1e3), seq(4e4, 1e7, by = 1e4))) +
#   theme(axis.ticks.y = element_line())

# plt_items <- ggplot(data = benchmark_items) +
#   geom_boxplot(mapping = aes(x = factor(nItems), y = jsTime, fill = 'js')) +
#   geom_boxplot(mapping = aes(x = factor(nItems), y = cppTime, fill = 'cpp')) + 
#   xlab('number of unique items in itemSet') + 
#   ylab('mining duration (ms)') + 
#   scale_y_continuous(trans = 'log10', breaks = c(seq(2e3, 5e3, by = 1e3), seq(4e4, 1e7, by = 1e4))) +
#   theme(axis.ticks.y = element_line())


newdf <- rbind(
  data.frame(
    x = benchmark_trans$nTrans,
    y = benchmark_trans$cppTime,
    class = 'cpp'
  ),
  data.frame(
    x = benchmark_trans$nTrans,
    y = benchmark_trans$jsTime,
    class = 'js'
  )
)


newdf2 <- rbind(
  data.frame(
    x = benchmark_items$nItems,
    y = benchmark_items$cppTime,
    class = 'cpp'
  ),
  data.frame(
    x = benchmark_items$nItems,
    y = benchmark_items$jsTime,
    class = 'js'
  )
)

options(scipen = 10000)
plt_trans <- ggplot(data = newdf, mapping = aes(x = factor(x), y = y/1000)) +
  geom_boxplot() +
  xlab('number of transactions') + 
  ylab('mining duration (sec)') + 
  theme(axis.ticks.y = element_line()) + 
  scale_y_continuous(breaks = c(seq(1e3, 5e3, by = 2e2), seq(4e4, 1e7, by = 1e4))/1000) + 
  facet_wrap(.~class, ncol = 2, scales = "free") #+ 
  # stat_summary(fun.y = mean, geom="point", mapping = aes(color = class), size=2) #+
  # stat_summary(fun.data = fun_mean, geom="text", vjust=-0.7)


plt_items <- ggplot(data = newdf2, mapping = aes(x = factor(x), y = y/1000)) +
  geom_boxplot() +
  xlab('number of unique items in itemSet') + 
  ylab('mining duration (sec)') + 
  theme(axis.ticks.y = element_line()) + 
  scale_y_continuous(breaks = c(seq(1e3, 5e3, by = 2e2), seq(4e4, 1e7, by = 3e3))/1000) +
  facet_wrap(.~class, ncol = 2, scales = "free")


ggsave('./plt_trans.png', plt_trans, width = 12 * 0.75, height = 7 * 0.75, units = 'in', dpi = 600);
ggsave('./plt_items.png', plt_items, width = 12 * 0.75, height = 7 * 0.75, units = 'in', dpi = 600);



# print(benchmark_trans$jsTime / benchmark_trans$cppTime)
# print(benchmark_items$jsTime / benchmark_items$cppTime)

plt_trans_ratio <- ggplot(data = benchmark_trans, mapping = aes(x = factor(nTrans), y = jsTime / cppTime)) + 
  geom_boxplot()
plt_items_ratio <- ggplot(data = benchmark_items, mapping = aes(x = factor(nItems), y = jsTime / cppTime)) + 
  geom_boxplot()

ggsave('./plt_trans_ratio.png', plt_trans_ratio, width = 12 * 0.75, height = 7 * 0.75, units = 'in', dpi = 600);
ggsave('./plt_items_ratio.png', plt_items_ratio, width = 12 * 0.75, height = 7 * 0.75, units = 'in', dpi = 600);
