require("ggplot2")
require("scales")
require("grid")

args <- commandArgs(trailingOnly = TRUE)
csvFile <- args[1]

theme_complete_bw <- function(base_size = 12, base_family = "") {
  theme(
    line =               element_line(colour = "black", size = 0.5, linetype = 1,lineend = "butt"),
    rect =               element_rect(fill = "white", colour = "black", size = 0.5, linetype = 1),
    text =               element_text(family = base_family, face = "plain",colour = "black", size = base_size,hjust = 0.5, vjust = 0.5, angle = 0, lineheight = 0.9),
    axis.text =          element_text(size = rel(0.7), colour = "grey50"),
    strip.text =         element_text(size = base_size * 0.7),
    legend.background =  element_blank(),
    legend.margin =      unit(0.2, "cm"),
    legend.key =         element_blank(),
    legend.key.height =  unit(0.8, "cm"),
    legend.key.width =   unit(0.8, "cm"),
    legend.text =        element_text(size = rel(1)),
    legend.text.align =  NULL,
    legend.title =       element_blank(),
    legend.title.align = NULL,
    legend.justification = "center",
    legend.box =         NULL,
    legend.position = "bottom",
    panel.background =   element_rect(fill = NA, colour = "grey", size = 1.3),
    panel.border =       element_blank(),
    panel.grid.major =   element_line(colour = "grey90", size = 0.7),
    panel.grid.minor =   element_line(colour = "grey90", size = 0.3),
    panel.margin =       unit(0.7, "lines"),
    strip.background =   element_rect(fill = NA, colour = NA),
    strip.text.x =       element_text(colour = "black", size = base_size * 0.8),
    strip.text.y =       element_text(colour = "black", size = base_size * 0.8, angle = -90),
    plot.background =    element_rect(colour = NA, fill = "white"),
    plot.title =         element_text(size = base_size * 1.2),
    plot.margin=         unit(c(0,0,0,0),"mm"),
    complete = TRUE
  )
}

d <- read.csv(csvFile,sep=";")

# compute additional values
d$space_percent <- d$spaceusage / d$textsize
d$time_per_op <- d$value/d$iterations

# plot
plot <- ggplot(d,aes(x=space_percent,y=time_per_op,shape=type,group=type,linetype=type))
plot <- plot + geom_point()
plot <- plot + geom_line()
plot <- plot + facet_grid(metric ~ file,scales="free_y")
plot <- plot + scale_y_continuous(name="Time per Operation [microsec]")
plot <- plot + scale_x_continuous(name="Space Usage [%]",labels=percent)
plot <- plot + theme_complete_bw()

ggsave(plot,file=paste(csvFile,".pdf",sep = ""))