glb !void srand()
{
    !int <64> current_time = @time(0);
    @srand(current_time); 
}

glb !int <64> rand()
{
    ret @rand();
}
